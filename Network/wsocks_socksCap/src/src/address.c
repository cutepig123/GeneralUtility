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
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "address.h"
#include "acl_cond.h"
#include "misc.h"

/*
 * Adddress fromat:
 * Host: 1.2.3.4
 * Service: 1.2.3.4:80
 * Range: 1.2.3.4-1.2.3.5 1.2.3.4-1.2.3.4 == Host: 1.2.3.4
 * Network: 1.2.3.4/24 1.2.3.4/255.255.255.0 1.2.3.4/32 == Host: 1.2.3.4
 */
address* parse_address(const char *str, int *eat, int *error)
{
	char buf[INET_ADDRSTRLEN];
	int retval;
	address *res = NULL;
	u_int32_t ip;
	const char *str_orig = str;

	if(eat)
		*eat = 0;
	if(error)
		*error = 0;
	if(!str)
		goto err;
	retval = valid_ip(str);
	if(!retval)
		goto err;
	res = malloc(sizeof(address));
	if(!res)
		goto err;
	memcpy(buf, str, retval);
	buf[retval] = '\0';
	if(inet_pton(AF_INET, buf, &ip) < 0)
		goto err;
	switch(str[retval]){
	case '\0':
	case ' ':
	case '\t':
	case '\n':
		res->type = ADDR_HOST;
		res->in.host.ip = ntohl(ip);
		break;
	case '/':
		res->type = ADDR_NETWORK;
		res->in.network.ip = ntohl(ip);
		str += retval + 1;
		retval = valid_ip(str);
		if(retval){ /* 192.168.2.0/255.255.255.0 */
			memcpy(buf, str, retval);
			buf[retval] = '\0';
			if(inet_pton(AF_INET, buf, &ip) < 0)
				goto err;
			res->in.network.mask = ntohl(ip);
			break;
		}
		/* 192.168.2.0/24 */
		retval = atoi(str);
		if(retval < 0 || retval > 32) goto err;
		if(retval == 32){
			res->type = ADDR_HOST;
			res->in.host.ip = ntohl(ip);
		}else{
			res->in.network.mask = num_to_netmask(retval);
		}
		break;
	case '-':
		res->type = ADDR_RANGE;
		res->in.range.begin = ntohl(ip);
		str += retval + 1;
		retval = valid_ip(str);
		if(!retval) goto err;
		memcpy(buf, str, retval);
		buf[retval] = '\0';
		if(inet_pton(AF_INET, buf, &ip) < 0)
			goto err;
		res->in.range.end = ntohl(ip);
		if(res->in.range.begin > res->in.range.end){
			goto err;
		}else if(res->in.range.begin == res->in.range.end){
			/* Just a host */
			res->type = ADDR_HOST;
			res->in.host.ip = ntohl(ip);
		}
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

/* just like snprintf */
int snprint_address(char *str, size_t size, address *addr)
{
	int len = 0, retval;
	char buf[INET_ADDRSTRLEN];
	u_int32_t ip;
	socklen_t buflen = sizeof(buf);

	if(!str || size < 0 || !addr) return -1;

	switch(addr->type){
	case ADDR_HOST:
		ip = htonl(addr->in.host.ip);
		if(!inet_ntop(AF_INET, &ip, buf, buflen))
			return -1;
		len = snprintf(str, size, "%s", buf);
		break;
	case ADDR_RANGE:
		ip = htonl(addr->in.range.begin);
		if(!inet_ntop(AF_INET, &ip, buf, buflen))
			return -1;
		len = snprintf(str, size, "%s-", buf);
		if(len < 0 || len >= size)
			return -1;
		str += len;
		size -= len;
		ip = htonl(addr->in.range.end);
		if(!inet_ntop(AF_INET, &ip, buf, buflen))
			return -1;
		retval = snprintf(str, size, "%s", buf);
		if(retval < 0 || retval >= size)
			return -1;
		len += retval;
		break;
	case ADDR_NETWORK:
		ip = htonl(addr->in.network.ip);
		if(!inet_ntop(AF_INET, &ip, buf, buflen))
			return -1;
		len = snprintf(str, size, "%s/%d", buf,
				netmask_to_num(addr->in.network.mask));
		break;
	default:
		return -1;
	}

	return len;
}

/* return 0 if sa matches addr. */
int match_address(struct sockaddr *sa, address *addr)
{
	u_int32_t ip;

	if(!addr) return -1;

	ip = ntohl(((struct sockaddr_in *)sa)->sin_addr.s_addr);
	switch(addr->type){
	case ADDR_HOST:
		if(ip == addr->in.host.ip)
			return 0;
		else
			return 1;
		break;
	case ADDR_RANGE:
		if(ip >= addr->in.range.begin
				&& ip <= addr->in.range.end)
			return 0;
		else
			return 1;
		break;
	case ADDR_NETWORK:
		if((ip ^ addr->in.network.ip) & addr->in.network.mask)
			return 1;
		else
			return 0;
		break;
	default:
		return -1;
	}

	return 0;
}

static acl_cond_module_t ac_address = {
	.name		= "address",
	.parse		= ACL_PARSE_PTR(&parse_address),
	.snprint	= ACL_SNPRINT_PTR(&snprint_address),
	.match		= ACL_MATCH_PTR(&match_address),
	.free		= ACL_FREE_PTR(&free)
};

ACL_COND_MODULE_DCLR(ac_address);
