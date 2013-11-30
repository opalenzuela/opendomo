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



#ifndef __COELACANTH_HASHTABLE_H__
#define __COELACANTH_HASHTABLE_H__

#include<stdio.h>
#include<stdlib.h>


unsigned int ht_hash(char *key, size_t ht_size)
{
   unsigned int h;
   unsigned char *p;

   h=0;
   for(p=(unsigned char*)key; *p!='\0'; p++)
      h = 31*h + *p;

   return h%ht_size;
}



#define TYPEDEF_HASHTABLE(name,type) \
\
typedef struct name##_node_t \
{ \
	char *key; \
	type value; \
	struct name##_node_t *next; \
} \
name##_node_t; \
\
typedef struct name##_t \
{ \
	size_t table_size; \
	name##_node_t **ht; \
	type defval; \
} \
name##_t; \
\
name##_t *name##_create(size_t table_size, type defval)\
{ \
	name##_t *s = malloc(sizeof(name##_t)); \
	if(!s) \
		return NULL; \
	s->table_size = table_size; \
	s->ht = malloc(sizeof(name##_node_t)*table_size); \
	s->defval = defval; \
	return s; \
} \
\
void name##_set(name##_t *ht, char *key, type value) \
{ \
	name##_node_t *n = NULL; \
	int found = 0; \
	unsigned int h = ht_hash(key, ht->table_size);\
	for(n=ht->ht[h]; n!=NULL; n=n->next) \
	{ \
		if(strcmp(key, n->key)==0) \
		{ \
			n->value = value; \
			found = 1; \
		} \
	} \
	if(!found) \
	{ \
		n = malloc(sizeof(name##_node_t)); \
		n->key = key; \
		n->value = value; \
		n->next = ht->ht[h]; \
		ht->ht[h] = n; \
	} \
} \
\
type name##_get(name##_t *ht, char *key) \
{ \
puts("--A--"); \
	name##_node_t *n = NULL; \
puts("--B--"); \
	unsigned int h = ht_hash(key, ht->table_size); \
puts("--C--"); \
printf("h: %d\n", h); \
	for(n=ht->ht[h]; n!=NULL; n=n->next) \
	{ \
puts("--D--"); \
printf("n: %d\n", n); \
printf("n val: %d\n", n->value); \
		if(strcmp(key, n->key)==0) \
			return n->value; \
puts("--E--"); \
	} \
	return ht->defval; \
} \
\
void name##_free(name##_t *ht) \
{ \
	if(!ht) \
		return; \
	int i; \
	for(i=0; i<ht->table_size; i++) \
	{ \
		name##_node_t *prev = NULL; \
	   name##_node_t *n = NULL; \
		for(n=ht->ht[i]; n!=NULL; n=n->next) \
		{ \
			if(prev) \
			{ \
				free(prev); \
			} \
			prev = n; \
		} \
		if(prev) \
		{ \
			free(prev); \
		} \
	} \
	if(ht); \
		free(ht); \
}

#endif





