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

#ifndef _SERVICE_H_
#define _SERVICE_H_

enum {
	SERV_PORT,
	SERV_RANGE
};

#define SERV_STRLEN	12

typedef struct _service
{
	unsigned char type;
	union {
		struct {
			u_int16_t port;
		} serv;
		struct {
			u_int16_t begin;
			u_int16_t end;
		} range;
	} data;
} service;
service* parse_service(const char *str, int *eat, int *error);
int snprint_service(char *str, size_t size, service *serv);
int match_service(struct sockaddr *sa, service *serv);

#endif /* _CHAIN_H_ */
