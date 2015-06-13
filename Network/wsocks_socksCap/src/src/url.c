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
#include <assert.h>
#include <string.h>

#include "url.h"
#include "misc.h"

short_url_t* short_url_new(void)
{
	short_url_t *surl;

	surl = malloc(sizeof(*surl));
	if(!surl)
		return NULL;
	memset(surl, 0, sizeof(*surl)); 

	return surl;
}

const char* short_url_to_string(short_url_t *surl)
{
	char *str;
	int len = 1;

	assert(surl && surl->host && valid_ip(surl->host)
			&& !(surl->password && !surl->user)
			&& surl->port >= 0 && surl->port < 65536);

	if(surl->user) len += strlen(surl->user) + 1;
	if(surl->password) len += strlen(surl->password) + 1;
	len += 16 + 6;
	str = malloc(len);
	if(!str) return NULL;
	snprintf(str, len, "%s%s%s%s%s:%d",
			surl->user ? surl->user : "",
			surl->password ? ":" : "",
			surl->password ? surl->password : "",
			surl->user ? "@" : "",
			surl->host,
			surl->port);

	return str;
}

int short_url_parse(short_url_t *surl, const char *url_str, int *eat)
{
	int len;
	const char *ptr, *url_str_orig;

	assert(surl && url_str);
	if(eat){
		url_str_orig = url_str;
		*eat = 0;
	}
	skip_blank(url_str);

	for(ptr = url_str;
			*ptr != '\0' && *ptr != ' ' && *ptr != '\t';
			ptr ++){
		if(*ptr == '@'){
			const char *ptr2;

			for(ptr2 = url_str; ptr2 < ptr; ptr2 ++){
				if(*ptr2 == ':'){
					len = ptr2 - url_str;
					surl->user = malloc(len + 1);
					if(!surl->user) goto err;
					snprintf(surl->user, len + 1, "%s", url_str);
					url_str += len + 1;
					len = ptr - url_str;
					surl->password = malloc(len + 1);
					if(!surl->password) goto err;
					snprintf(surl->password, len + 1, "%s", url_str);
					url_str  += len + 1;
					goto parse_host;
				}
			}
			len = ptr - url_str;
			surl->user = malloc(len + 1);
			if(!surl->user) goto err;
			snprintf(surl->user, len + 1, "%s", url_str);
			url_str += len + 1;
			goto parse_host;
		}
	}

parse_host:
	len = valid_ip(url_str);
	if(!len) goto err;
	surl->host = malloc(len + 1);
	if(!surl->host) goto err;
	snprintf(surl->host, len + 1, "%s", url_str);
	url_str += len;
	if(*url_str == ':'){
		surl->port = atoi(++url_str);
		if(surl->port <= 0 || surl->port > 65535)
			goto err;
		while(*url_str >= '0' && *url_str <= '9')
			url_str ++;
	}
	if(eat)
		*eat = url_str - url_str_orig;

	return 0;

err:
	return -1;
}

short_url_t* short_url_new_from_string(const char *url_str, int *eat)
{
	short_url_t *surl;

	assert(url_str);
	surl = short_url_new();
	if(!surl) return NULL;

	if(short_url_parse(surl, url_str, eat) < 0){
		short_url_free(surl);
		return NULL;
	}

	return surl;
}

void short_url_free(short_url_t *url)
{
	assert(url);

	if(url->host) free(url->host);
	if(url->user) free(url->user);
	if(url->password) free(url->password);
	free(url);
}

#if 0
int main(int argc, char *argv[])
{
	short_url_t *surl;
	const char *url;
	int len;

	surl = short_url_new_from_string(argv[1], &len);
	if(!surl)
		return -1;
	printf("surl->user = %s\n"
		   "surl->password = %s\n"
		   "surl->host = %s\n"
		   "surl->port = %d\n"
		   "eat_len = %d\n",
		   surl->user ? surl->user : "",
		   surl->password ? surl->password : "",
		   surl->host ? surl->host : "",
		   surl->port,
		   len);
	url = short_url_to_string(surl);
	printf("url = %s\n", url);
	short_url_free(surl);
	free(url);

	return 0;
}
#endif
