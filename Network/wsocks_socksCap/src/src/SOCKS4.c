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
#include "misc.h"

typedef struct wsocks_prot_socks4
{
	wsocks_operations_t	*op;
	struct sockaddr_in	serv_addr;
	socklen_t			addrlen;
	const char			*auth;
	const char			*url_str;
} wsocks_prot_socks4_t;

#define WSOCKS_PROT_SOCKS4_PTR(x) ((wsocks_prot_socks4_t*)(x))

static int socks4_connect(wsocks_ctx_t *wc, const struct sockaddr *serv_addr,
		socklen_t addrlen)
{
	wsocks_prot_socks4_t *wp;
	struct sockaddr_in *serv_addr_in;
	unsigned char buf[4096];
	unsigned char *ptr;
	int slen, retval;

	wp = WSOCKS_PROT_SOCKS4_PTR(wc->prot);
	serv_addr_in = (struct sockaddr_in *)serv_addr;

	/* send CONNECT command */
	dprintf("send CONNECT command\n");
	buf[0] = 0x04;
	buf[1] = 0x01;
	*(u_int16_t*)&buf[2] = serv_addr_in->sin_port;
	*(u_int32_t*)&buf[4] = serv_addr_in->sin_addr.s_addr;
	if (wp->auth) {
		strncpy((char*)&buf[8], wp->auth, sizeof(buf) - 8);
		buf[sizeof(buf) - 1] = '\0';
	} else {
		buf[8] = '\0';
	}
	slen = 8 + strlen((const char*)&buf[8]) + 1;
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
	slen = 8;
	while(slen > 0){
		retval = recv_ni(wc->fd, ptr, slen, 0);
		if(retval <= 0){
			goto err;
		}
		slen -= retval;
		ptr += retval;
	}
	if (buf[0] != 0
			|| buf[1] != 90)
		goto err;
	wc->local.sin_family = AF_INET;
	wc->local.sin_port = *(u_int16_t*)&buf[2];
	wc->local.sin_addr.s_addr = *(u_int32_t*)&buf[4];
	wc->state |= WSOCKS_STATE_BINDED;
	retval = 0;

done:
	return retval;

err:
	errno = ECONNREFUSED;
	retval = -1;
	goto done;
}

static wsocks_operations_t wsocks_operations_socks4 = {
	.connect	= socks4_connect
};

static void SOCKS4_free(void *priv)
{
	wsocks_prot_socks4_t *wp = WSOCKS_PROT_SOCKS4_PTR(priv);

	if(wp->auth) free((void*)wp->auth);
	if(wp->url_str) free((void*)wp->url_str);
}

static void* SOCKS4_parse(const char *str, int *eat, int *error)
{
	short_url_t *surl = NULL;
	wsocks_prot_socks4_t *wp = NULL;

	if(eat) *eat = 0;
	if(error) *error = 0;
	surl = short_url_new_from_string(str, eat);
	if(!surl) goto err;
	if(!surl->port) surl->port = 1080;
	wp = malloc(sizeof(*wp));
	if(!wp) goto err;
	memset(wp, 0, sizeof(*wp));
	wp->url_str = short_url_to_string(surl);
	if(!wp->url_str)
		goto err;
	wp->op = &wsocks_operations_socks4;
	wp->serv_addr.sin_family = AF_INET;
	if(inet_pton(AF_INET, surl->host,
				&wp->serv_addr.sin_addr.s_addr) <= 0)
		goto err;
	wp->serv_addr.sin_port = htons(surl->port);
	wp->addrlen = sizeof(struct sockaddr_in);
	if(surl->user){
		wp->auth = strdup(surl->user);
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
	if(wp) SOCKS4_free(wp);
	if(error) *error = -1;
	wp = NULL;
	goto done;
}

static int SOCKS4_snprint(char *str, size_t size, void *priv)
{
	wsocks_prot_socks4_t *wp;

	wp = WSOCKS_PROT_SOCKS4_PTR(priv);

	return snprintf(str, size, "%s", wp->url_str);
}

static int SOCKS4_target(void *param, void *priv)
{
	wsocks_ctx_t *wc;
	wsocks_prot_socks4_t *wp;

	dprintf("SOCKS4_target()\n");
	wc = WSOCKS_CTX_PTR(param);
	wp = WSOCKS_PROT_SOCKS4_PTR(priv);
	assert(wc && wc->op);

	switch(wc->cmd){
	case WSOCKS_CMD_CONNECT:
		if(wc->op->connect(wc, (struct sockaddr*)&wp->serv_addr,
					wp->addrlen) < 0){
			dprintf("SOCKS4_target()::connect() error.\n");
			return -1;
		}
		wc->op = wp->op;
		wc->prot = (void*)wp;
		break;
	default:
		break;
	}
	dprintf("~SOCKS4_target()\n");
	return 0;
}

static acl_act_module_t ac_SOCKS4 = {
	.name		= "SOCKS4",
	.parse		= ACL_PARSE_PTR(&SOCKS4_parse),
	.snprint	= ACL_SNPRINT_PTR(&SOCKS4_snprint),
	.target		= ACL_TARGET_PTR(&SOCKS4_target),
	.free		= ACL_FREE_PTR(&SOCKS4_free),
};

ACL_ACT_MODULE_DCLR(ac_SOCKS4);
