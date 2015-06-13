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
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "misc.h"
#include "acl_cond.h"
#include "service.h"

service* parse_service(const char *str, int *eat, int *error)
{
	service *res = NULL;
	int val;
	const char *str_orig = str;

	if(eat)
		*eat = 0;
	if(error)
		*error = 0;
	if(!str)
		goto err;

	val = atoi(str);
	if(val < 0 || val > 0xffff)
		goto err;
	while(*str >= '0' && *str <= '9') str ++;
	res = malloc(sizeof(*res));
	if(!res)
		goto err;
	switch(*str){
	case '\0':
	case ' ':
	case '\t':
	case '\n':
		res->type = SERV_PORT;
		res->data.serv.port = val;
		break;
	case '-':
		res->type = SERV_RANGE;
		res->data.range.begin = val;
		str ++;
		val = atoi(str);
		if(val < 0 || val > 0xffff) goto err;
		res->data.range.end = val;
		if(res->data.range.begin == res->data.range.end){
			res->type = SERV_PORT;
			res->data.serv.port = val;
		}else if(res->data.range.begin > res->data.range.end)
			goto err;
		break;
	default:
		goto err;
	}

	if(eat){
		skip_word(str);
		*eat = str - str_orig;
	}
	return res;
err:
	if(res)
		free(res);
	if(error)
		*error = -1;
	return NULL;
}

int snprint_service(char *str, size_t size, service *serv)
{
	int len;
	if(!str || size < 0 || !serv) return -1;

	switch(serv->type){
	case SERV_PORT:
		len = snprintf(str, size, "%d", serv->data.serv.port);
		break;
	case SERV_RANGE:
		len = snprintf(str, size, "%d-%d",
				serv->data.range.begin,
				serv->data.range.end);
		break;
	default:
		return -1;
	}

	return len;
}

int match_service(struct sockaddr *sa, service *serv)
{
	u_int16_t port;

	if(!serv) return -1;

	port = ntohs(((struct sockaddr_in*)sa)->sin_port);
	switch(serv->type){
	case SERV_PORT:
		if(port == serv->data.serv.port)
			return 0;
		else
			return 1;
	case SERV_RANGE:
		if(port >= serv->data.range.begin
				&& port <= serv->data.range.end)
			return 0;
		else
			return 1;
	default:
		return -1;
	}

	return 0;
}

static acl_cond_module_t ac_service = {
	.name		= "service",
	.parse		= ACL_PARSE_PTR(&parse_service),
	.snprint	= ACL_SNPRINT_PTR(&snprint_service),
	.match		= ACL_MATCH_PTR(&match_service),
	.free		= ACL_FREE_PTR(&free)
};

ACL_COND_MODULE_DCLR(ac_service);
