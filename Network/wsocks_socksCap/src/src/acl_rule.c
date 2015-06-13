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
#include <assert.h>

#include "list.h"
#include "acl_rule.h"

acl_rule_t* acl_rule_new(void)
{
	acl_rule_t *ar;

	ar = malloc(sizeof(*ar));
	if(!ar)
		return NULL;
	list_init(&ar->cond, NULL);
	list_init(&ar->act, NULL);

	return ar;
}

void acl_rule_free(acl_rule_t *ar)
{
	assert(ar);

	list_free(&ar->cond);
	list_free(&ar->act);
	free(ar);
}
