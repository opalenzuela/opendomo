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



#ifndef __OD_DS_STR_H__
#define __OD_DS_STR_H__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

/*
 * @func: str_assign_str()
 * @desc: Assign c-string value 
 *
 * @param char *o: string
 * @param size_t olen: string lenght
 * @param const char *s: value to assign
 * @return: no return value
 */
void str_assign_str(char *o, size_t olen, const char *s);

/*
 * @func: string_assign_int()
 * @desc: Assign int value
 * 
 * @param char *o: string
 * @param size_t olen: string lenght
 * @param int value: integer value
 * @return: no return value
 */
void str_assign_int(char *o, size_t olen, int value);

/*
 * @func: string_assign_float()
 * @desc: Assign float value
 *
 * @param char *o: string
 * @param size_t olen: string lenght
 * @param float value: value
 * @return: no return vallue
 */
void str_assign_float(char *o, size_t olen, float value);

/*
 * @func: str_to_int()
 * @desc: Cast string to int
 * 
 * @param char*: string
 * @return int: int value
 */
int str_to_int(char *o);

/*
 * @func: string_to_float()
 * @desc: Cast string to float 
 * 
 * @param char*: string
 * @return float: float value
 */
float str_to_float(char *o);

/*
 * @func: str_trim()
 * Remove whitespace from the start and end of the string
 *
 * @param str: string
 */
void str_trim(char *str);

/**
 * Split string separated by 'token'. Copy first string in 'res'.
 *
 * @param str: string to be splitted
 * @param token: token used as separator
 * @param res: result
 * @param reslen: buffer length.
 * @return char*: pointer to next string
 */
char* str_split(char *str, const char *token, char *res, size_t reslen);

/*
 * @func: str_replace(str, strlen, pattern, replacement)
 * @desc: Replace pattern by replacement.
 * 
 * @param char *str: string
 * @param size_t strlen: str len.
 * @param const char *pattern: pattern
 * @param const char *replacement: replacement
 * @return: no return value
 */
void str_replace(char *str, size_t strlen, 
                 const char *pattern, const char *replacement);
  
/*
 * @func: str_regex_match()
 * @desc: Return true if regex match with str.
 * 
 * @param const char *str: string
 * @param const char *regex: regular expression
 * @return: int, true if it match
 */

int str_regex_match(const char *str, const char *regex);

/*
 * @func: str_append()
 * @desc: Append string
 * 
 * @param char *o: string
 * @param size_t olen: string lenght
 * @return: no return value
 */
void str_append(char *o, size_t olen, const char *str);

/*
 * @func: str_cut()
 * @desc: Get a substring
 * 
 * @param char *o: string
 * @param size_t olen: string lenght
 * @param size_t begin: begin index 
 * @param size_t end: end index 
 * @return: no return value
 */
void str_cut(char *o, size_t olen, size_t begin, size_t end);

/*
 * @func: str_left()
 * @desc: Get the left side of string
 * 
 * @param char *o: string
 * @param size_t olen: string lenght
 * @return: no return value
 */
#define str_left(o,olen,len) str_cut(o, olen, 0, len);

/*
 * @func: str_right()
 * @desc: Get the right side of string
 * 
 * @param char *o: string
 * @param size_t olen: string lenght
 * @return: no return value
 */
#define str_right(o,olen,len) str_cut(o, olen, strlen(o)-len, strlen(o));

/*
 * @func: str_find()
 * @desc: Find next occurrence of pattern
 *
 * @param char *o: string
 * @return size_t: pattern position, -1 if not found
 */
size_t str_find(char *o, const char *pattern);

/*
 * @func: str_starts_with()
 * @desc: check the begining string
 *
 * @param char *o: string
 * @return bool: true if starts with
 */
#define str_starts_with(o,s) (str_find(o,s)==0?1:0)

/*
 * @func: str_ends_with()
 * @desc: check the ending string
 *
 * @param char *o: string
 * @return bool: true if ends with
 */
#define str_ends_with(o,s) (str_find(o,s)==strlen(o)-strlen(s)?1:0)

/*
 * @func: str_lpad()
 * @desc: Pads the left side of a string with a specific character
 *
 * @param char *o: string
 * @param size_t olen: string lenght
 * @param char padd_char: padding character
 * @param size_t total_len: new string length
 * @return: no return value
 */
void str_lpad(char *o, size_t olen, char padd_char, size_t total_len);

/*
 * @func: str_rpad()
 * @desc: Pads the right side of a string with a specific character
 *
 * @param char *o: string
 * @param size_t olen: string lenght
 * @param char padd_char: padding character
 * @param size_t total_len: new string length
 * @return: no return value
 */
void str_rpad(char *o, size_t olen, char padd_char, size_t total_len);

/*
 * @func: str_to_upper()
 * @desc: to upper conversion
 *
 * @param char *o: string
 * @param size_t olen: string lenght
 * @return: no return value
 */
void str_to_upper(char *o, size_t olen);

/*
 * @func: str_to_lower()
 * @desc: to lower conversion
 *
 * @param char *o: string
 * @param size_t olen: string lenght
 * @return: no return value
 */
void str_to_lower(char *o, size_t olen);


/*
 * @func: str_random()
 * @desc: Generate a random string using only validchars
 *
 * @param char *o: string
 * @param size_t olen: string lenght
 * @param size_t rndlen: random string lenght
 * @return: no return value
 */
int str_random(char *o, size_t olen, int rndlen, const char *validchars);



#endif


