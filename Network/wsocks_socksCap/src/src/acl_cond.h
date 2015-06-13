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

#ifndef _ACL_COND_H_
#define _ACL_COND_H_

#include "list.h"
#include "acl.h"

typedef int (acl_match_t) (void *parameter, void *acl_priv);

#define ACL_MATCH_PTR(x) ((acl_match_t*)(x))

typedef struct acl_cond_module
{
	const char		*name;
	acl_parse_t		*parse;
	acl_match_t		*match;
	acl_snprint_t	*snprint;
	acl_free_t		*free;
} acl_cond_module_t;

#define ACL_COND_MODULE_PTR(x) ((acl_cond_module_t*)(x))

typedef acl_cond_module_t* (acl_cond_module_register_t)(void);

#define ACL_COND_MODULE_DCLR(x) \
	acl_cond_module_t* acl_cond_module_register(void) \
	{ \
		return &(x); \
	}

acl_cond_module_t* request_acl_cond_module(
		acl_t *acl,
		const char *pr,
		int len);

typedef struct acl_cond
{
	acl_cond_module_t	*mod;
	void				*priv;
} acl_cond_t;

#define ACL_COND_PTR(x) ((acl_cond_t*)(x))

acl_cond_t* acl_cond_new(void);
void acl_cond_free(acl_cond_t *ac);

#endif /* _ACL_COND_H_ */
