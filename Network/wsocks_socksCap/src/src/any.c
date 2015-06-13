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

#include "acl_cond.h"

static void* any_parse(const char *str, int *eat, int *error)
{
	if(eat) *eat = 0;
	if(error) *error = 0;

	return NULL;
}

static int any_snprint(char *str, size_t size, void *priv)
{
	return snprintf(str, size, " ");
}

static int any_match(void *param, void *priv)
{
	return 0;
}

static void any_free(void *priv)
{
}

static acl_cond_module_t ac_any = {
	.name		= "any",
	.parse		= ACL_PARSE_PTR(&any_parse),
	.snprint	= ACL_SNPRINT_PTR(&any_snprint),
	.match		= ACL_MATCH_PTR(&any_match),
	.free		= ACL_FREE_PTR(&any_free),
};

ACL_COND_MODULE_DCLR(ac_any);
