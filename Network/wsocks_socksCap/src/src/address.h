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

#ifndef _ADDRESS_H_
#define _ADDRESS_H_

#include <sys/types.h>

enum {
	ADDR_HOST,
	ADDR_RANGE,
	ADDR_NETWORK
};

#define ADDR_STRLEN		32

/* NOTE: all the ip and port are in host order. */
typedef struct _address
{
	unsigned char type;
	union {
		struct {
			u_int32_t ip;
		} host;
		struct {
			u_int32_t begin;
			u_int32_t end;
		} range;
		struct {
			u_int32_t ip;
			u_int32_t mask;
		} network;
	} in;
} address;
address* parse_address(const char *str, int *eat, int *error);
int snprint_address(char *str, size_t size, address *addr);
int match_address(struct sockaddr *sa, address *addr);

#endif /* _ADDRESS_H_ */
