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

#ifndef _WSOCKS_H_
#define _WSOCKS_H_

#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "config.h"

#ifdef DEBUG
	#define dprintf(fmt, args...) \
		do{ \
			fprintf(stderr, "[%s:%d] " fmt, __FILE__, __LINE__, ##args); \
		}while(0)
#else
	#define dprintf(fmt, args...) \
		do{ \
		}while(0)
#endif

#ifndef WSOCKS_PREFIX
#define WSOCKS_PREFIX ""
#endif

#define WSOCKS_LIB_PATH WSOCKS_PREFIX "/lib/wsocks"
#define WSOCKS_COND_PATH WSOCKS_LIB_PATH "/cond"
#define WSOCKS_ACT_PATH WSOCKS_LIB_PATH "/act"
#define WSOCKS_CFG_PATH WSOCKS_PREFIX "/etc"
#define WSOCKS_BIN_PATH WSOCKS_PREFIX "/bin"

typedef enum
{
	WSOCKS_CMD_SOCKET,
	WSOCKS_CMD_CONNECT,
	WSOCKS_CMD_BIND
} wsocks_cmd_t;

typedef enum
{
	WSOCKS_STATE_INIT = 1,
	WSOCKS_STATE_CONNECTED = 2,
	WSOCKS_STATE_BINDED = 4
} wsocks_state_t;

typedef struct wsocks_ctx wsocks_ctx_t;

typedef struct wsocks_operations
{
	int	(*connect)(wsocks_ctx_t *wc, const struct sockaddr *serv_addr,
			socklen_t addrlen);
} wsocks_operations_t;

struct wsocks_ctx
{
	wsocks_cmd_t		cmd;
	wsocks_state_t		state;
	int					fd;
	wsocks_operations_t	*op;
	void				*prot;
	struct sockaddr_in	local;
	struct sockaddr_in	remote;
	volatile int		refcnt;
};

wsocks_ctx_t* wsocks_ctx_new(void);
wsocks_ctx_t* wsocks_ctx_get(wsocks_ctx_t *wc);
void wsocks_ctx_free(wsocks_ctx_t *wc);

#define WSOCKS_CTX_PTR(x) ((wsocks_ctx_t*)(x))

typedef struct wsocks_ctx_map
{
	int					fd;
	wsocks_ctx_t		*ctx;
} wsocks_ctx_map_t;

void wsocks_ctx_map_free(wsocks_ctx_map_t *wcm);

#define WSOCKS_CTX_MAP_PTR(x) ((wsocks_ctx_map_t*)(x))

#endif /* _WSOCKS_H_ */
