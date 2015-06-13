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

#ifndef _ACL_H_
#define _ACL_H_

#include <sys/types.h>
#include <stdio.h>

#include "list.h"

typedef void* (acl_parse_t) (const char *parameter, int *eat, int *err);
typedef int (acl_snprint_t) (char *buf, size_t size, void *acl_priv);
typedef void (acl_free_t) (void *acl_priv);

#define ACL_PARSE_PTR(x) ((acl_parse_t*)(x))
#define ACL_SNPRINT_PTR(x) ((acl_snprint_t*)(x))
#define ACL_FREE_PTR(x) ((acl_free_t*) (x))

typedef struct acl
{
	struct list rule;
	struct list cond_mod;
	struct list act_mod;
	struct list cond_path;
	struct list act_path;
} acl_t;

void acl_init(acl_t *acl);
acl_t* acl_new(void);
void acl_free(acl_t *acl);
int acl_parse(acl_t *acl, const char *rule_str);
int acl_do(acl_t *acl, void *cond_param, void *act_param);
int acl_load_from_FILE(acl_t *acl, FILE *fh);
int acl_load_to_FILE(acl_t *acl, FILE *fh);
int acl_load_from_file(acl_t *acl, const char *file);
int acl_load_to_file(acl_t *acl, const char *file);

#endif /* _ACL_H_ */
