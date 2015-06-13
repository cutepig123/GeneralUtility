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

#ifndef _ACL_ACT_H_
#define _ACL_ACT_H_

#include "acl.h"

typedef int (acl_target_t) (void *parameter, void *acl_priv);

#define ACL_TARGET_PTR(x) ((acl_target_t*)(x))

typedef struct acl_act_module
{
	const char		*name;
	acl_parse_t		*parse;
	acl_target_t	*target;
	acl_snprint_t	*snprint;
	acl_free_t		*free;
} acl_act_module_t;

#define ACL_ACT_MODULE_PTR(x) ((acl_act_module_t*)(x))

typedef acl_act_module_t* (acl_act_module_register_t)(void);

#define ACL_ACT_MODULE_DCLR(x) \
	acl_act_module_t* acl_act_module_register(void) \
	{ \
		return &(x); \
	}

acl_act_module_t* request_acl_act_module(
		acl_t *acl,
		const char *pr,
		int len);

typedef struct acl_act
{
	acl_act_module_t	*mod;
	void				*priv;
} acl_act_t;

#define ACL_ACT_PTR(x) ((acl_act_t*)(x))

acl_act_t* acl_act_new(void);
void acl_act_free(acl_act_t *aa);

#endif /* _ACL_ACT_H_ */
