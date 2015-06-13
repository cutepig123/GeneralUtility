/*
 *  Copyright (C) 2006- xiaosuo <xiaosuo@gmail.com> 
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#define __USE_GNU
#include <dlfcn.h>
#undef __USE_GNU
#ifdef __WSOCKS_THREAD
#include <pthread.h>
#endif
#ifdef __WSOCKS_DNS
#include <arpa/nameser.h>
#include <resolv.h>
#endif

#include "wsocks.h"
#include "list.h"
#include "acl.h"
#include "misc.h"

#ifdef __WSOCKS_THREAD
static pthread_mutex_t __wsocks_ctx_mutex;
static void __wsocks_cleanup_lock(void *arg)
{
	pthread_mutex_t *mutex;

	mutex = (pthread_mutex_t*)arg;
	pthread_mutex_unlock(mutex);
}
#define WSOCKS_CTX_LOCK() \
	do{ \
		pthread_cleanup_push(__wsocks_cleanup_lock, \
			&__wsocks_ctx_mutex) \
		pthread_mutex_lock(&__wsocks_ctx_mutex)
#define WSOCKS_CTX_UNLOCK() \
		pthread_cleanup_pop(1); \
	}while(0)
#else
#define WSOCKS_CTX_LOCK() do{}while(0)
#define WSOCKS_CTX_UNLOCK() do{}while(0)
#endif

static struct list wsocks_ctx_map_list;
static acl_t *wsocks_acl;

static int (*op_socket)(int domain, int type, int protocol);
static int (*op_connect)(int  sockfd,  const  struct  sockaddr  *serv_addr,
		socklen_t addrlen);
static int (*op_bind)(int sockfd, const struct sockaddr *my_addr, socklen_t addrlen);
static int (*op_close)(int fd);

static int (*op_getpeername)(int s, struct sockaddr *name, socklen_t *namelen);
static int (*op_getsockname)(int s, struct sockaddr *name, socklen_t *namelen);
static int (*op_dup)(int oldfd);
static int (*op_dup2)(int oldfd, int newfd);
static int (*op_fclose)(FILE *fh);
static FILE* (*op_freopen)(const char *path, const char *mode, FILE *stream);
#ifdef __WSOCKS_DNS
static int (*op_res_init)(void);
static struct hostent* (*op_gethostbyname)(const char *name);
static struct hostent* (*op_gethostbyaddr)(const void *addr, int len, int type);
static struct hostent* (*op_gethostent)(void);
static struct hostent* (*op_gethostbyname2)(const char *name, int af);
static int (*op_gethostent_r)(
		struct hostent *ret, char *buf, size_t buflen,
		struct hostent **result, int *h_errnop);
static int (*op_gethostbyname_r)(const char *name,
		struct hostent *ret, char *buf, size_t buflen,
		struct hostent **result, int *h_errnop);
static int (*op_gethostbyname2_r)(const char *name, int af,
		struct hostent *ret, char *buf, size_t buflen,
		struct hostent **result, int *h_errnop);
static int (*op_getaddrinfo)(const char *node, const char *service,
		const struct addrinfo *hints,
		struct addrinfo **res);
static int (*op_getnameinfo)(const struct sockaddr *sa, socklen_t salen,
		char *host, size_t hostlen,
		char *serv, size_t servlen, int flags);
#endif

void _init(void)
{
	dprintf("libwsocks starting ...\n");

#ifdef _op_reslove
	#error "macro _op_reslove exists"
#endif

#define _op_reslove(dlname) \
	do{ \
		dprintf("resolve [%s]\n", #dlname); \
		if((op_##dlname = dlsym(RTLD_NEXT, #dlname)) == NULL){ \
			fprintf(stderr, "Can't resolve symbol %s: %s.\n", #dlname, dlerror()); \
			abort(); \
		} \
	}while(0)
	_op_reslove(socket);
	_op_reslove(connect);
	_op_reslove(bind);
	_op_reslove(close);
	_op_reslove(fclose);
	_op_reslove(getpeername);
	_op_reslove(getsockname);
	_op_reslove(dup);
	_op_reslove(dup2);
	_op_reslove(freopen);
#ifdef __WSOCKS_DNS
	_op_reslove(res_init);
#endif
#undef op_resolve

	list_init(&wsocks_ctx_map_list, NULL);
	dprintf("acl_load_from_file\n");
	wsocks_acl = acl_new();
	if(!wsocks_acl){
		perror("fatal error acl_new()");
		abort();
	}
	list_append(&wsocks_acl->cond_path, WSOCKS_COND_PATH, NULL);
	list_append(&wsocks_acl->act_path, WSOCKS_ACT_PATH, NULL);
	if(acl_load_from_file(wsocks_acl, WSOCKS_CFG_PATH "/libwsocks.acl") < 0){
		fprintf(stderr, "libwsocks's acl isn't loaded successfully.\n");
		abort();
	}
#ifdef __WSOCKS_THREAD
	if(pthread_mutex_init(&__wsocks_ctx_mutex, NULL) != 0){
		perror("fatal error pthread_mutex_init()");
		abort();
	}
#endif

	dprintf("libwsocks is initialed.\n");
}

void _fini(void)
{
#ifdef __WSOCKS_THREAD
	if(pthread_mutex_destroy(&__wsocks_ctx_mutex) != 0){
		perror("fatal error pthread_mutex_destroy()");
		abort();
	}
#endif
	list_free(&wsocks_ctx_map_list);
	acl_free(wsocks_acl);
	dprintf("unload library libwsocks.\n");
}

wsocks_ctx_t* wsocks_ctx_new(void)
{
	wsocks_ctx_t *wc;

	wc = malloc(sizeof(*wc));
	if(!wc)
		return NULL;
	memset(wc, 0, sizeof(*wc));
	wc->refcnt = 1;

	return wc;
}

wsocks_ctx_t* wsocks_ctx_get(wsocks_ctx_t *wc)
{
	assert(wc);

	wc->refcnt += 1;

	return wc;
}

void wsocks_ctx_free(wsocks_ctx_t *wc)
{
	assert(wc);

	wc->refcnt -= 1;
	if(wc->refcnt == 0)
		free(wc);
}

void wsocks_ctx_map_free(wsocks_ctx_map_t *wcm)
{
	assert(wcm);

	if(wcm->ctx)
		wsocks_ctx_free(wcm->ctx);
	free(wcm);
}

static wsocks_ctx_map_t* wsocks_ctx_map_get_by_fd(int fd)
{
	struct list *i;
	wsocks_ctx_map_t *wcm, *retval = NULL;

	WSOCKS_CTX_LOCK();
	list_for_each(i, &wsocks_ctx_map_list){
		wcm = WSOCKS_CTX_MAP_PTR(i->data);
		if(wcm->fd == fd){
			retval = wcm;
			break;
		}
	}
	WSOCKS_CTX_UNLOCK();

	return retval;
}

static wsocks_ctx_map_t* wsocks_ctx_map_hold_by_fd(int fd)
{
	struct list *i;
	wsocks_ctx_map_t *wcm, *retval = NULL;

	WSOCKS_CTX_LOCK();
	list_for_each(i, &wsocks_ctx_map_list){
		wcm = WSOCKS_CTX_MAP_PTR(i->data);
		if(wcm->fd == fd){
			wsocks_ctx_get(wcm->ctx);
			retval = wcm;
		}
	}
	WSOCKS_CTX_UNLOCK();

	return retval;
}

static void wsocks_ctx_map_remove_by_fd(int fd)
{
	struct list *i;
	wsocks_ctx_map_t *wcm;

	WSOCKS_CTX_LOCK();
	list_for_each(i, &wsocks_ctx_map_list){
		wcm = WSOCKS_CTX_MAP_PTR(i->data);
		if(wcm->fd == fd){
			list_remove(&wsocks_ctx_map_list, i->data);
			break;
		}
	}
	WSOCKS_CTX_UNLOCK();
}

/* XXX If we called bind before,
 * we should bind it too here. */
static void wsocks_rebuild_socket(wsocks_ctx_t *wc)
{
	int newsock;

	assert(wc);
	newsock = op_socket(AF_INET, SOCK_STREAM, 0);
	if(newsock < 0){
		perror("fatal error when create a new socket.");
		abort();
	}
	while(op_dup2(newsock, wc->fd) < 0){
		if(errno == EINTR) continue;
		perror("fatal error when duplicate socket.");
		abort();
	}
	while(op_close(newsock) < 0){
		if(errno == EINTR) continue;
		perror("fatal error when close the socket.");
		abort();
	}
	if(wc->state & WSOCKS_STATE_BINDED){
		socklen_t addrlen = sizeof(struct sockaddr_in);
		if(bind(wc->fd, (struct sockaddr*)&wc->local, addrlen) < 0){
			abort();
		}
	}
}

static int raw_connect(wsocks_ctx_t *wc, const struct sockaddr *serv_addr,
		socklen_t addrlen)
{
	int retval;

	while((retval = op_connect(wc->fd, serv_addr, addrlen)) < 0){
		if(errno != EINTR)
			return retval;
	}

	return retval;
}

static wsocks_operations_t wsocks_operations_raw = {
	.connect	= raw_connect
};

static struct
{
	wsocks_operations_t	*op;
	void				*prot_priv;
} wsocks_prot_raw = {
	.op			= &wsocks_operations_raw,
	.prot_priv	= NULL
};

int socket(int domain, int type, int protocol)
{
	wsocks_ctx_t *wc;
	wsocks_ctx_map_t *wcm;
	int retval;

	dprintf("socket(%d, %d, %d)\n", domain, type, protocol);
	if(domain != AF_INET
			|| type != SOCK_STREAM
			|| (protocol != 0 && protocol != IPPROTO_IP
				&& protocol != IPPROTO_TCP))
		return op_socket(domain, type, protocol);

	retval = op_socket(domain, type, protocol);
	if(retval < 0) return retval;
	wc = wsocks_ctx_new();
	if(!wc){
		errno = ENOMEM;
		return -1;
	}
	wc->state = WSOCKS_STATE_INIT;
	wcm = malloc(sizeof(*wcm));
	if(!wcm){
		errno = ENOMEM;
		wsocks_ctx_free(wc);
		return -1;
	}
	wcm->fd = retval;
	wcm->ctx = wc;
	WSOCKS_CTX_LOCK();
	list_append(&wsocks_ctx_map_list, wcm,
		(void(*)(void*))(&wsocks_ctx_map_free));
	WSOCKS_CTX_UNLOCK();

	return retval;
}

int close(int fd)
{
	int retval;

	dprintf("close(%d)\n", fd);
	retval = op_close(fd);
	if(retval < 0){
		return retval;
	}
	wsocks_ctx_map_remove_by_fd(fd);

	return retval;
}

int fclose(FILE *fh)
{
	int retval, fd;

	assert(fh);
	dprintf("fclose(0x%p)\n", fh);
	fd = fileno(fh);
	retval = op_fclose(fh);
	if(retval != 0){
		return retval;
	}
	wsocks_ctx_map_remove_by_fd(fd);

	return retval;
}

int  connect(int  sockfd,  const  struct  sockaddr  *serv_addr,
		socklen_t addrlen)
{
	wsocks_ctx_t *wc;
	wsocks_ctx_map_t *wcm;
	int retval, flags;

	dprintf("connect(%d, 0x%p, %d)\n", sockfd, serv_addr, addrlen);
	wcm = wsocks_ctx_map_get_by_fd(sockfd);
	if(!wcm)
		return op_connect(sockfd, serv_addr, addrlen);
	wc = wcm->ctx;
	wc->fd = wcm->fd;
	if(wc->state & WSOCKS_STATE_CONNECTED){
		errno = EISCONN;
		return -1;
	}
	wc->cmd = WSOCKS_CMD_CONNECT;
	wc->op = wsocks_prot_raw.op;
	wc->prot = &wsocks_prot_raw;
	memcpy(&wc->remote, serv_addr, sizeof(struct sockaddr_in));
	flags = block_save_flags(sockfd);
	if(acl_do(wsocks_acl, (void*)serv_addr, (void*)wc) < 0){
		retval = -1;
		wc->state = WSOCKS_STATE_INIT;
		goto out;
	}

	retval = wc->op->connect(wc, serv_addr, addrlen);
	if(retval == 0){
		wc->state &= ~WSOCKS_STATE_INIT;
		wc->state |= WSOCKS_STATE_CONNECTED;
	}else{
		wc->state = WSOCKS_STATE_INIT;
	}

out:
	/* we got an error when using this socket to communication
	 * and the original socket can't be used any more. if you
	 * want to use it, you must create a new one and duplicate
	 * it to the old one. */
	if(retval != 0)
		wsocks_rebuild_socket(wc);
	restore_flags(sockfd, flags);

	return retval;
}

int getpeername(int s, struct sockaddr *name, socklen_t *namelen)
{
	wsocks_ctx_t *wc;
	wsocks_ctx_map_t *wcm;

	dprintf("getpeername(%d, 0x%p, 0x%p)\n", s, name, namelen);
	wcm = wsocks_ctx_map_get_by_fd(s);
	if(!wcm)
		return op_getpeername(s, name, namelen);
	wc = wcm->ctx;
	if(!(wc->state & WSOCKS_STATE_CONNECTED)){
		errno = ENOTCONN;
		return -1;
	}
	if(!name){
		errno = EFAULT;
		return -1;
	}
	if(!namelen || *namelen < sizeof(struct sockaddr_in)){
		*namelen = 0;
		errno = EINVAL;
		return -1;
	}
	memcpy(name, &wc->remote, sizeof(struct sockaddr_in));
	*namelen = sizeof(struct sockaddr_in);

	return 0;
}

int bind(int sockfd, const struct sockaddr *my_addr, socklen_t addrlen)
{
	int retval;
	wsocks_ctx_t *wc;
	wsocks_ctx_map_t *wcm;

	dprintf("bind(%d, 0x%p, %d)\n", sockfd, my_addr, addrlen);
	retval = op_bind(sockfd, my_addr, addrlen);
	if(retval < 0) return retval;
	wcm = wsocks_ctx_map_get_by_fd(sockfd);
	if(!wcm) return retval;
	wc = wcm->ctx;
	wc->state &= ~WSOCKS_STATE_INIT;
	wc->state |= WSOCKS_STATE_BINDED;
	/* because the bind port maybe 0, we should
	 * get the address info through the system call
	 * getsockname(). */
	op_getsockname(sockfd, (struct sockaddr*)&wc->local, &addrlen);

	return retval;
}

int getsockname(int s, struct sockaddr *name, socklen_t *namelen)
{
	wsocks_ctx_t *wc;
	wsocks_ctx_map_t *wcm;

	dprintf("getsockname(%d, 0x%p, 0x%p)\n", s, name, namelen);
	wcm = wsocks_ctx_map_get_by_fd(s);
	if(!wcm)
		return op_getsockname(s, name, namelen);
	wc = wcm->ctx;
	if(!name){
		errno = EFAULT;
		return -1;
	}
	if(!namelen || *namelen < sizeof(struct sockaddr_in)){
		errno = EINVAL;
		return -1;
	}
	if(wc->state & WSOCKS_STATE_BINDED){
		memcpy(name, &wc->local, sizeof(struct sockaddr_in));
		*namelen = sizeof(struct sockaddr_in);
		return 0;
	}

	return op_getsockname(s, name, namelen);
}

int dup(int oldfd)
{
	wsocks_ctx_map_t *wcm, *new_wcm;
	int retval;

	dprintf("dup(%d)\n", oldfd);
	retval = op_dup(oldfd);
	if(retval < 0) return retval;
	wcm = wsocks_ctx_map_hold_by_fd(oldfd);
	if(!wcm) return retval;
	new_wcm = malloc(sizeof(*new_wcm));
	if(!new_wcm){
		op_close(retval);
		WSOCKS_CTX_LOCK();
		wsocks_ctx_free(wcm->ctx);
		WSOCKS_CTX_UNLOCK();
		errno = ENOMEM;
		return -1;
	}
	new_wcm->fd = retval;
	new_wcm->ctx = wcm->ctx;
	WSOCKS_CTX_LOCK();
	list_append(&wsocks_ctx_map_list, new_wcm,
			(void(*)(void*))(&wsocks_ctx_map_free));
	WSOCKS_CTX_UNLOCK();

	return retval;
}

int dup2(int oldfd, int newfd)
{
	int retval;
	wsocks_ctx_map_t *wcm, *new_wcm;

	dprintf("dup2(%d, %d)\n", oldfd, newfd);
	/* XXX we assume that the newfd is closed whether 
	 * the routine dup2 is success or not. */
	retval = close(newfd);
	if(retval < 0 && errno != EBADF)
			return retval;
	retval = op_dup2(oldfd, newfd);
	if(retval < 0) return retval;
	wcm = wsocks_ctx_map_hold_by_fd(oldfd);
	if(!wcm) return retval;
	new_wcm = malloc(sizeof(*new_wcm));
	if(!new_wcm){
		wsocks_ctx_free(wcm->ctx);
		op_close(retval);
		errno = ENOMEM;
		return -1;
	}
	new_wcm->fd = retval;
	new_wcm->ctx = wcm->ctx;
	WSOCKS_CTX_LOCK();
	list_append(&wsocks_ctx_map_list, new_wcm,
			(void(*)(void*))(&wsocks_ctx_map_free));
	WSOCKS_CTX_UNLOCK();

	return retval;
}

FILE* freopen(const char *path, const char *mode, FILE *stream)
{
	int retval;

	dprintf("freopen(%s, 0x%p, 0x%p)\n", path, mode, stream);
	retval = fclose(stream);
	if(retval < 0 && errno != EBADF)
		return NULL;
	return op_freopen(path, mode, stream);
}

#ifdef __WSOCKS_DNS
int res_init(void)
{
	int retval;

	dprintf("res_init()\n");
	retval = op_res_init();
	/* force using TCP protocol for DNS resolving */
	_res.options |= RES_USEVC;

	return retval;
}
#endif
