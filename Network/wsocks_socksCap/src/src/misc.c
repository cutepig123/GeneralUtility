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

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#include "misc.h"

/*
 * @brief check whether a ip address string pointed by str. is valid.
 * @return false if not. or the length of the ip address string/
 */
int valid_ip(const char *str)
{
	const char *ptr;
	int retval, n;

	for(ptr = str, n = 1; *ptr; ptr ++){
		retval = atoi(ptr);
		if(retval < 0 || retval > 255){
			return 0;
		}
		while(*ptr >= '0' && *ptr <= '9') ptr ++;
		if(n != 4){
			if(*ptr != '.') return 0;
			n ++;
		}else{
			return ptr - str;
		}
	}

	return 0;
}

u_int32_t num_to_netmask(int num)
{
	u_int32_t retval = 0x00000000;
	int i;

	for(i = 0; i < num; i ++)
		retval = (retval >> 1) | 0x80000000;

	return retval;
}

int netmask_to_num(u_int32_t mask)
{
	int retval = 0;

	while((mask & 0x80000000) && retval < 32){
		retval ++;
		mask <<= 1;
	}

	return retval;
}

int block_save_flags(int fd)
{
	int flags;

	flags = fcntl(fd, F_GETFL, 0);
	if(flags & O_NONBLOCK){
		flags &= ~O_NONBLOCK;
		fcntl(fd, F_SETFL, flags);
		flags |= O_NONBLOCK;
	}

	return flags;
}

int restore_flags(int fd, int flags)
{
	return fcntl(fd, F_SETFL, flags);
}

ssize_t send_ni(int s, const void *buf, size_t len, int flags)
{
	int retval;

	while((retval = send(s, buf, len,  flags)) < 0){
		if(errno != EINTR)
			return retval;
	}

	return retval;
}

ssize_t recv_ni(int s, void *buf, size_t len, int flags)
{
	int retval;

	while((retval = recv(s, buf, len, flags)) < 0){
		if(errno != EINTR)
			return retval;
	}

	return retval;
}
