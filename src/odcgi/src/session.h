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


#ifndef __SESSION_H__
#define __SESSION_H__


#include<stdio.h>

int session_gen_random(char *rnd, size_t bytes_len);
int session_check_pass(const char *clear_pass, const char *encrypted_pass);
void session_set_ids(const char *user);
int session_is_valid_user(const char *user, const char *pass);
int session_is_valid_sessid(const char *sessid);
int session_get_user(const char *sessid, char *user, size_t len);
int session_str_is_alnum(const char *str);
int session_store_session(const char* user, const char* sessid);
void session_delete_session(const char* user);


#endif




