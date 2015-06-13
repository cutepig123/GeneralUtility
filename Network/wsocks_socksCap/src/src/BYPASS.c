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
#include <string.h>

#include "acl_act.h"

static void* BYPASS_parse(const char *str, int *eat, int *error)
{
	if(eat) *eat = 0;
	if(error) *error = 0;

	return NULL;
}

static int BYPASS_snprint(char *str, size_t size, void *priv)
{
	return snprintf(str, size, " ");
}

static int BYPASS_target(void *param, void *priv)
{
	return 0;
}

static void BYPASS_free(void *priv)
{
}

static acl_act_module_t ac_BYPASS = {
	.name		= "BYPASS",
	.parse		= ACL_PARSE_PTR(&BYPASS_parse),
	.snprint	= ACL_SNPRINT_PTR(&BYPASS_snprint),
	.target		= ACL_TARGET_PTR(&BYPASS_target),
	.free		= ACL_FREE_PTR(&BYPASS_free),
};

ACL_ACT_MODULE_DCLR(ac_BYPASS);
