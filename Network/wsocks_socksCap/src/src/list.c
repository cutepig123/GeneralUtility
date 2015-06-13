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

#include "list.h"

void list_init(struct list *li, void (*dtor)(void*))
{
	if(!li) return;

	li->prev = li;
	li->next = li;
	li->data = NULL;
	li->dtor = dtor;
}

struct list* list_new(void)
{
	struct list *li;

	li = malloc(sizeof(*li));
	if(!li) return NULL;

	list_init(li, &free);

	return li;
}

void list_free(struct list *li)
{
	struct list *it, *tmp;

	if(!li) return;

	list_for_each_safe(it, tmp, li){
		if(it->data && it->dtor)
			it->dtor(it->data);
		free(it);
	}
	if(li->dtor)
		li->dtor(li);
}

struct list* list_append(struct list *li, void *data, void (*dtor)(void*))
{
	struct list *new;

	if(!li || !data) return NULL;

	new = malloc(sizeof(*new));
	if(!new) return NULL;
	new->data = (void*)data;
	new->dtor = dtor;
	list_link_before(li, new);

	return li;
}

struct list* list_insert(struct list *li, void *data, void (*dtor)(void*))
{
	struct list *new;

	if(!li || !data) return NULL;

	new = malloc(sizeof(*new));
	if(!new) return NULL;
	new->data = (void*)data;
	new->dtor = dtor;
	list_link_after(li, new);

	return li;
}

void list_remove(struct list *li, void *data)
{
	struct list *it;

	if(!li || !data) return;

	list_for_each(it, li){
		if(it->data == data){
			list_unlink(it);
			if(it->dtor)
				it->dtor(it->data);
			free(it);
			break;
		}
	}

	return;
}

#if 0
int main(int argc, char *argv[])
{
	struct list li;
	struct list *it;
	int i;

	list_init(&li, NULL);
	for(i = 1; i < argc; i ++){
		list_append(&li, argv[i], NULL);
	}
	list_for_each_bw(it, &li){
		printf("%s\n", (char*)it->data);
	}
	printf("-----\n");
	list_remove(&li, argv[2]);
	list_for_each(it, &li){
		printf("%s\n", (char*)it->data);
	}
	list_free(&li);

	return 0;
}
#endif
