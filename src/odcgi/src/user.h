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


#ifndef __USER_H__
#define __USER_H__


#define USER_ERROR_SIZE 256

#define USER_GROUP_ID   1001
#define USER_DEFAULT_ID   1001


/** user_t data type */
typedef struct
{
   char error[USER_ERROR_SIZE];
}
user_t;


void user_add(user_t *o, char *username, volatile char *passwd);
void user_del(user_t *o, char *username);
void user_set_password(user_t *o, char *username, volatile char* passwd);

#define user_get_error(o)  (o)->error
#define user_has_error(o)  (strlen((o)->error)>0?1:0)


#endif




