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

#ifndef _MISC_H_
#define _MISC_H_

#include <sys/types.h>

int valid_ip(const char *str);
u_int32_t num_to_netmask(int num);
int netmask_to_num(u_int32_t mask);
int block_save_flags(int fd);
int restore_flags(int fd, int flags);
ssize_t send_ni(int s, const void *buf, size_t len, int flags);
ssize_t recv_ni(int s, void *buf, size_t len, int flags);

#define skip_blank(ptr) \
	do{ \
		while(*ptr == ' ' || *ptr == '\t') \
			ptr ++; \
	}while(0)

#define skip_word(ptr) \
	do{ \
		while(*ptr != '\0' && *ptr != ' ' && *ptr !=  '\t') \
			ptr ++; \
	}while(0)

#endif /* _MISC_H */
