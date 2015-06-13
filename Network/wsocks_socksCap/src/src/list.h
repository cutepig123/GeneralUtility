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

#ifndef _LIST_H_
#define _LIST_H_

/* common list macro */
#define list_for_each(it, list) \
	for(it = (list)->next; it != (list); it = it->next)

#define list_for_each_safe(it, tmp, list) \
	for(it = (list)->next, tmp = it->next; it != (list); \
			it = tmp, tmp = it->next)

#define list_for_each_bw(it, list) \
	for(it = (list)->prev; it != (list); it = it->prev)

#define list_for_each_bw_safe(it, tmp, list) \
	for(it = (list)->prev, tmp = it->prev; it != (list); \
			it = tmp, tmp = it->prev)

#define list_link_before(list, new) \
	do{ \
		(list)->prev->next = (new); \
		(new)->prev = (list)->prev; \
		(list)->prev = (new); \
		(new)->next = (list); \
	}while(0)

#define list_link_after(list, new) \
	do{ \
		(list)->next->prev = (new); \
		(new)->next = (list)->next; \
		(list)->next = (new); \
		(new)->prev = (list); \
	}while(0)

#define list_unlink(list) \
	do{ \
		(list)->prev->next = (list)->next; \
		(list)->next->prev = (list)->prev; \
	}while(0)

#define list_empty(list) \
	((list)->next == (list) &&(list)->prev == (list))

struct list
{
	struct list	*prev;
	struct list	*next;
	void		*data;
	void		(*dtor)(void *);
};

void list_init(struct list *li, void (*dtor)(void*));
struct list* list_new(void);
void list_free(struct list *li);
struct list* list_append(struct list *li, void *data, void (*dtor)(void*));
struct list* list_insert(struct list *li, void *data, void (*dtor)(void*));
void list_remove(struct list *li, void *data);

#define list_add_before(list, data, dtor) \
	list_append(list, data, dtor)

#define list_add_after(list, data, dtor) \
	list_insert(list, data, dtor)

#endif /* _LIST_H_ */
