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
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "acl_act.h"
#include "wsocks.h"
#include "url.h"
#include "base64.h"
#include "misc.h"

typedef struct wsocks_prot_http
{
	wsocks_operations_t	*op;
	struct sockaddr_in	serv_addr;
	socklen_t			addrlen;
	const char			*auth;
	const char			*url_str;
} wsocks_prot_http_t;

#define WSOCKS_PROT_HTTP_PTR(x) ((wsocks_prot_http_t*)(x))

static int http_connect(wsocks_ctx_t *wc, const struct sockaddr *serv_addr,
		socklen_t addrlen)
{
	wsocks_prot_http_t *wp;
	struct sockaddr_in *serv_addr_in;
	char buf[4096], addr_buf[INET_ADDRSTRLEN];
	char *ptr;
	int slen, ok, retval;

	wp = WSOCKS_PROT_HTTP_PTR(wc->prot);
	serv_addr_in = (struct sockaddr_in *)serv_addr;
	if(!inet_ntop(AF_INET, &serv_addr_in->sin_addr.s_addr, addr_buf,
				INET_ADDRSTRLEN)){
		errno = EINVAL;
		return -1;
	}

	/* send CONNECT command */
	dprintf("send CONNECT command\n");
	slen = snprintf(buf, 4096,
			"CONNECT %s:%d HTTP/1.1\r\n"
			"%s%s%s"
			"\r\n",
			addr_buf,
			ntohs(serv_addr_in->sin_port),
			wp->auth ? "Proxy-Authorization: Basic " : "",
			wp->auth ? wp->auth : "",
			wp->auth ? "\r\n" : "");
	dprintf("\n%s", buf);
	ptr = buf;
	while(slen > 0){
		retval = send_ni(wc->fd, ptr, slen, 0);
		if(retval <= 0){
			goto err;
		}
		ptr += retval;
		slen -= retval;
	}
	/* parse the reply */
	dprintf("get the reply.\n");
	ptr = buf;
	slen = 0;
	ok = 0;
	while(1){
		retval = recv_ni(wc->fd, ptr, 1, 0);
		if(retval <= 0){
			goto err;
		}
		slen += 1;
		if(*ptr == '\n'){
			*(ptr + 1) = '\0';
			dprintf("%s", buf);
			if(buf[0] == '\n'
					|| (buf[0] == '\r' && buf[1] == '\n')){
				/* get the end of reply */
				if(ok){
					retval = 0;
					goto done;
				}else{
					goto err;
				}
			}
			if(strstr(buf, "HTTP/1.") && strstr(buf, "200")){
				ok = 1;
			}
			ptr = buf;
			slen = 0;
			continue;
		}
		ptr += 1;
		/* too long return message? */
		if(slen >= 4095){
			goto err;
		}
	}

done:
	return retval;

err:
	errno = ECONNREFUSED;
	retval = -1;
	goto done;
}

static wsocks_operations_t wsocks_operations_http = {
	.connect	= http_connect
};

static void HTTP_free(void *priv)
{
	wsocks_prot_http_t *wp = WSOCKS_PROT_HTTP_PTR(priv);

	if(wp->auth) free((void*)wp->auth);
	if(wp->url_str) free((void*)wp->url_str);
}

static void* HTTP_parse(const char *str, int *eat, int *error)
{
	short_url_t *surl = NULL;
	wsocks_prot_http_t *wp = NULL;

	if(eat) *eat = 0;
	if(error) *error = 0;
	surl = short_url_new_from_string(str, eat);
	if(!surl) goto err;
	if(!surl->port) surl->port = 8080;
	wp = malloc(sizeof(*wp));
	if(!wp) goto err;
	memset(wp, 0, sizeof(*wp));
	wp->url_str = short_url_to_string(surl);
	if(!wp->url_str)
		goto err;
	wp->op = &wsocks_operations_http;
	wp->serv_addr.sin_family = AF_INET;
	if(inet_pton(AF_INET, surl->host,
				&wp->serv_addr.sin_addr.s_addr) <= 0)
		goto err;
	wp->serv_addr.sin_port = htons(surl->port);
	wp->addrlen = sizeof(struct sockaddr_in);
	if(surl->user){
		char buf[256];
		snprintf(buf, sizeof(buf),
				"%s%s%s",
				surl->user,
				surl->password ? ":" : "",
				surl->password ? surl->password : "");
		wp->auth = base64_encode(buf, -1, NULL);
		if(!wp->auth)
			goto err;
	}else{
		wp->auth = NULL;
	}

	short_url_free(surl);
done:
	return wp;

err:
	if(surl) short_url_free(surl);
	if(wp) HTTP_free(wp);
	if(error) *error = -1;
	wp = NULL;
	goto done;
}

static int HTTP_snprint(char *str, size_t size, void *priv)
{
	wsocks_prot_http_t *wp;

	wp = WSOCKS_PROT_HTTP_PTR(priv);

	return snprintf(str, size, "%s", wp->url_str);
}

static int HTTP_target(void *param, void *priv)
{
	wsocks_ctx_t *wc;
	wsocks_prot_http_t *wp;

	dprintf("HTTP_target()\n");
	wc = WSOCKS_CTX_PTR(param);
	wp = WSOCKS_PROT_HTTP_PTR(priv);
	assert(wc && wc->op);

	switch(wc->cmd){
	case WSOCKS_CMD_CONNECT:
		if(wc->op->connect(wc, (struct sockaddr*)&wp->serv_addr,
					wp->addrlen) < 0){
			dprintf("HTTP_target()::connect() error.\n");
			return -1;
		}
		wc->op = wp->op;
		wc->prot = (void*)wp;
		break;
	default:
		break;
	}
	dprintf("~HTTP_target()\n");
	return 0;
}

static acl_act_module_t ac_HTTP = {
	.name		= "HTTP",
	.parse		= ACL_PARSE_PTR(&HTTP_parse),
	.snprint	= ACL_SNPRINT_PTR(&HTTP_snprint),
	.target		= ACL_TARGET_PTR(&HTTP_target),
	.free		= ACL_FREE_PTR(&HTTP_free),
};

ACL_ACT_MODULE_DCLR(ac_HTTP);
