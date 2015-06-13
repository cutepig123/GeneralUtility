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

#ifndef _URL_H_
#define _URL_H_

typedef struct short_url
{
	char	*host;
	int		port;
	char	*user;
	char	*password;
} short_url_t;

short_url_t* short_url_new(void);
int short_url_parse(short_url_t *surl, const char *url_str, int *eat);
const char* short_url_to_string(short_url_t *surl);
short_url_t* short_url_new_from_string(const char *url_str, int *eat);
void short_url_free(short_url_t *url);

#endif /* _URL_H_ */
