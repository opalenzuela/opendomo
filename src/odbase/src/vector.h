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


#ifndef __COELACANTH_VECTOR_H__
#define __COELACANTH_VECTOR_H__


#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#define TYPEDEF_VECTOR(name, type) \
typedef struct name##_t \
{ \
   size_t size; \
   type  *vector; \
	int use_cmp_fn; \
	int(*cmp_fn)(const type, const type); \
} \
name##_t; \
 \
size_t name##_size(name##_t *v) \
{ \
   return v->size; \
} \
 \
int name##_add(name##_t *v, type value) \
{ \
   v->size++; \
   if(!v->vector) \
      v->vector = malloc(sizeof(type)*(v->size)); \
   else \
      v->vector = realloc(v->vector, sizeof(type)*(v->size)); \
   if(!v->vector) \
      return -1; \
   v->vector[v->size-1] = value; \
   return 0; \
} \
 \
type name##_get(name##_t *v, size_t idx) \
{ \
   return v->vector[idx]; \
} \
 \
void name##_set(name##_t *v, size_t idx, type value) \
{ \
   v->vector[idx] = value; \
} \
 \
name##_t* name##_create() \
{ \
	name##_t * v = malloc(sizeof(name##_t)); \
	if(!v) \
		return NULL; \
	v->size = 0; \
	v->vector = NULL; \
	v->use_cmp_fn = 0; \
	return v; \
} \
 \
void name##_destroy(name##_t *v) \
{ \
   if(v->vector) \
	{ \
      free(v->vector); \
	} \
} \
\
int name##_cmp(type a, type b) \
{ \
	return (a-b); \
} \
void name##_set_cmp_fn(name##_t *v, int(*fn)(const type, const type)) \
{ \
	v->use_cmp_fn = 1; \
	v->cmp_fn =  fn; \
} \
\
void name##_quicksort(name##_t *v, int first, int last) \
{ \
	int i = first; \
	int j = last; \
	type tmp; \
	type p; \
   \
	if(v->use_cmp_fn) \
	{ \
		p = name##_get(v, first); \
	} \
	else \
	{ \
		p = name##_get(v, first); \
	} \
	\
	do \
	{ \
		if(v->use_cmp_fn) \
		{ \
			while(v->cmp_fn(name##_get(v, i), p) < 0) i++; \
			while(v->cmp_fn(name##_get(v, j), p) > 0) j--; \
		} \
		else \
		{ \
			while(name##_cmp(name##_get(v, i), p) < 0) i++; \
			while(name##_cmp(name##_get(v, j), p) > 0) j--; \
	   } \
		\
		if(i<=j) \
		{ \
			tmp = v->vector[j]; \
			v->vector[j] = v->vector[i]; \
			v->vector[i] = tmp; \
			i++; \
			j--; \
		} \
	} \
	while(i<=j); \
   \
	if(first<j) name##_quicksort(v, first, j); \
	if(last>i)  name##_quicksort(v, i, last); \
} \
\
void name##_sort(name##_t *v) \
{ \
	name##_quicksort(v, 0, name##_size(v)-1); \
}


#endif





