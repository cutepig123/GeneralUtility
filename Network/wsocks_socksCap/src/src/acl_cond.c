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
#include <dlfcn.h>
#include <assert.h>
#include <string.h>

#include "misc.h"
#include "list.h"
#include "acl_cond.h"

/* request the acl condition module through the module name.
 * if the module isn't pressent in the acl, load it dynamic,
 * and add it to the acl for using it */
acl_cond_module_t* request_acl_cond_module(
		acl_t *acl,
		const char *ptr,
		int len)
{
	struct list *i;
	acl_cond_module_t *acm;
	void *handle;
	acl_cond_module_register_t *acm_reg;
	char name[32];
	char path[256];

	/* find it in the acl */
	list_for_each(i, &acl->cond_mod){
		acm = ACL_COND_MODULE_PTR(i->data);
		if(strlen(acm->name) != len)
			continue;
		if(strncmp(acm->name, ptr, len) == 0)
			return acm;
	}

	if(len >= sizeof(name))
		return NULL;
	snprintf(name, len + 1, "%s", ptr);
	 /* find it in the cond_path */
	list_for_each(i, &acl->cond_path){
		snprintf(path, sizeof(path), "%s/%s.so",
				(const char*)(i->data),
				name);
		handle = dlopen(path, RTLD_LAZY);
		if(handle)
			goto add;
	}

	return NULL;

add:
	acm = NULL;
	acm_reg = dlsym(handle, "acl_cond_module_register");
	if(!acm_reg)
		goto done;
	acm = acm_reg();
	if(!acm)
		goto done;
	list_append(&acl->cond_mod, acm, NULL);

done:
	if(!acm && dlclose(handle) != 0)
		abort();

	return acm;
}

acl_cond_t* acl_cond_new(void)
{
	acl_cond_t *ac;

	ac = malloc(sizeof(*ac));
	if(!ac){
		return NULL;
	}
	memset(ac, 0, sizeof(*ac));

	return ac;
}

void acl_cond_free(acl_cond_t *ac)
{
	assert(ac);

	if(ac->mod && ac->mod->free)
		ac->mod->free(ac->priv);
	
	free(ac);
}
