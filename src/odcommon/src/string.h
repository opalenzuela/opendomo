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


#ifndef __COELACANTH_STRING_H__
#define __COELACANTH_STRING_H__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include <sys/types.h>
#include <regex.h>



/* ---------------------------------------------------------------------------
 *   String Data Type
 * ------------------------------------------------------------------------ */

/*
 * @type: string_t
 * @desc: string
 */
typedef struct string_t
{
   /* Pointer to C string */
   char *str;

   /* Vector size */
   size_t size;
}
string_t;

/*
 * @func: string_alloc()
 * @desc: Create a new string object
 *
 * @param s: new string
 * @return: string type
 */
string_t *string_alloc(const char *s);

/*
 * @func: string_free()
 * @desc: Free allocated string
 *
 * @param s: dynamic string string 
 * @return: no return value
 */
void string_free(string_t *s);

/*
 * @func: string_assign_str()
 * @desc: Assign c-string value 
 *
 * @param string_t *o: string_t object
 * @param const char *s: value to assign
 * @return: no return value
 */
void string_assign_str(string_t *o, const char *s);

/*
 * @func: string_assign_int()
 * @desc: Assign int value
 * 
 * @param string_t *o: string_t object
 * @param int value: integer value
 * @return: no return value
 */
void string_assign_int(string_t *o, int value);

/*
 * @func: string_assign_float()
 * @desc: Assign float value
 *
 * @param string_t *o: string_t object
 * @param float value: value
 * @return: no return vallue
 */
void string_assign_float(string_t *o, float value);

/*
 * @func: string_to_int()
 * @desc: Cast string to int
 * 
 * @param string_t *: string
 * @return int: int value
 */
int string_to_int(string_t *o);

/*
 * @func: string_to_float()
 * @desc: Cast string to float 
 * 
 * @param string_t*: string
 * @return float: float value
 */
float string_to_float(string_t *o);

/*
 * @func: string_trim()
 * @desc: Remove whitespace from the start and end of the string
 *
 * @param string_t *: string object
 * @return: no return value
 */
void string_trim(string_t *o);

/*
 * @func: string_replace()
 * @desc: Replace pattern by replacement.
 * 
 * @param string_t *: string object
 * @param const char *pattern: pattern
 * @param const char *replacement: replacement
 * @return: no return value
 */
void string_replace(string_t *o, const char *pattern, const char *replacement);

/*
 * @func: string_append()
 * @desc: Append string
 * 
 * @param string_t *: string object
 * @param str: value to append
 * @return: no return value
 */
void string_append(string_t *o, const char *str);

/*
 * @func: string_cut()
 * @desc: Get a substring
 * 
 * @param string_t *: string object
 * @param size_t begin: begin index 
 * @param size_t end: end index 
 * @return: no return value
 */
void string_cut(string_t *o, size_t begin, size_t end);

/*
 * @func: string_left()
 * @desc: Get the left side of string
 * 
 * @param string_t *: string object
 * @param size_t len: lenght
 * @return: no return value
 */
#define string_left(o,len) string_cut(o, 0, len);

/*
 * @func: string_right()
 * @desc: Get the right side of string
 * 
 * @param string_t *: string object
 * @param size_t len: lenght
 * @return: no return value
 */
#define string_right(o,len) string_cut(o, o->size-len, o->size);

/*
 * @func: string_find()
 * @desc: Find next occurrence of pattern
 *
 * @param string_t *: string object
 * @param const char *pattern: pattern
 * @return size_t: pattern position, -1 if not found
 */
size_t string_find(string_t *o, const char *pattern);

/*
 * @func: string_starts_with()
 * @desc: check the begining string
 *
 * @param string_t *: string object
 * @param const char *pattern: pattern
 * @return bool: true if starts with
 */
#define string_starts_with(o,s) (string_find(o,s)==0?1:0)

/*
 * @func: string_ends_with()
 * @desc: check the ending string
 *
 * @param string_t *: string object
 * @param const char *pattern: pattern
 * @return bool: true if ends with
 */
#define string_ends_with(o,s) (string_find(o,s)==o->size-strlen(s)?1:0)

/*
 * @func: string_lpad()
 * @desc: Pads the left side of a string with a specific character
 *
 * @param string_t *: string object
 * @param char padd_char: padding character
 * @param size_t total_len: new string length
 * @return: no return value
 */
void string_lpad(string_t *o, char padd_char, size_t total_len);

/*
 * @func: string_rpad()
 * @desc: Pads the right side of a string with a specific character
 *
 * @param string_t *: string object
 * @param char padd_char: padding character
 * @param size_t total_len: new string length
 * @return: no return value
 */
void string_rpad(string_t *o, char padd_char, size_t total_len);

/*
 * @func: string_compare()
 * @desc: Compare strings like strcmp()
 *
 * @param string_t *a: string object
 * @param string_t *b: string object
 * @return int: result
 */
int string_compare(const void *a, const void *b);
/*
 * @func: string_compare_reverse()
 * @desc: Compare strings like strcmp() in reverse order
 *
 * @param string_t *a: string object
 * @param string_t *b: string object
 * @return int: result
 */
int string_compare_reverse(const void *a, const void *b);




/* ---------------------------------------------------------------------------
 *   String Vector Data Type
 * ------------------------------------------------------------------------ */

/*
 * @type: string_vector_t
 * @desc: vector of strings
 */
typedef struct string_vector_t
{
   /* Vector elements */
   string_t **v;

   /* Vector size */
   size_t size;
}
string_vector_t;

/*
 * @func: string_vector_alloc()
 * @desc: Allocate memory and create a vector of strings
 *
 * @param s: new vector
 * @return: no return value
 */
string_vector_t *string_vector_alloc();

/*
 * @func: string_vector_free()
 * @desc: Free allocated vector and his elements
 *
 * @param s: vector
 * @return: no return value
 */
void string_vector_free(string_vector_t *sv);

/*
 * @func: string_vector_add()
 * @desc: Add a string to vector
 *
 * @param vs: vector
 * @param string: string 
 * @return: no return value
 */
void string_vector_add(string_vector_t *vs, string_t *o);

/**
 * @func: Split string separated by 'token'.
 *
 * @param vs: vector
 * @param str: string to be splitted
 * @param token: token used as separator
 * @return: no return value
 */
void string_split(string_vector_t *sv, const char *str, const char *token);


void string_sort(string_vector_t *sv, int reverse);


int string_regex_match(string_t *o,  const char *regex);

void string_regex(string_vector_t *sv, const char *string, const char *regex);





#endif





