/*****************************************************************************
 *  This file is part of the OpenDomo project.
 *  Copyright(C) 2011 OpenDomo Services SL
 *  
 *  Daniel Lerch Hostalot <dlerch@opendomo.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/


#ifndef __COELACANTH_STACK_H__
#define __COELACANTH_STACK_H__

#include<stdio.h>
#include<stdlib.h>
#include<assert.h>


#define TYPEDEF_STACK(name,type) \
typedef struct name##_node_t \
{ \
   type value; \
   struct name##_node_t *next; \
} \
name##_node_t; \
\
typedef struct name##_t \
{ \
	size_t size; \
	name##_node_t *head; \
} \
name##_t; \
\
name##_t *name##_create() \
{ \
	name##_t *s = malloc(sizeof(name##_t)); \
	if(!s) \
		return NULL; \
	s->size = 0; \
	s->head = NULL; \
	return s; \
} \
\
size_t name##_size(name##_t *s) \
{ \
	return s->size; \
} \
\
void name##_push(name##_t *s, type value) \
{ \
	name##_node_t *node = malloc(sizeof(name##_node_t)); \
	node->value = value; \
	node->next = s->head; \
	s->head = node; \
	s->size++; \
} \
\
type name##_get(name##_t *s) \
{ \
	assert(s->size>0); \
	return s->head->value; \
} \
\
void name##_pop(name##_t *s) \
{ \
	if(s->size==0) \
		return; \
	name##_node_t *head = s->head; \
   s->head = s->head->next; \
   free(head); \
	s->size--; \
} \
\
void name##_free(name##_t *s) \
{ \
	if(!s) \
		return; \
	\
	name##_node_t *prev = NULL; \
	name##_node_t *it = NULL; \
	for(it=s->head; it!=NULL; it=it->next) \
	{ \
		if(prev) \
		{ \
			free(prev); \
		} \
		prev = it; \
	} \
	\
	if(prev) \
	{ \
		free(prev); \
	} \
	\
	free(s); \
}



#endif





