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


#ifndef __CGI_H__
#define __CGI_H__


#define CGI_ERROR_SIZE 256

#include "coelacanth/string.h"


/** cgi_t data type */
typedef struct
{
   int  server_port;
   string_t *server_protocol;
   string_t *request_method;
   string_t *path_info;
   string_t *path_translated;
   string_t *script_name;
   string_t *query_string;
   string_t *remote_host;
   string_t *remote_addr;
   string_t *auth_type;
   string_t *remote_user;
   string_t *remote_ident;
   string_t *content_type;
   string_t *decoded_url;
   int  content_length;
   char error[CGI_ERROR_SIZE];
}
cgi_t;


cgi_t *cgi_alloc();
void cgi_free(cgi_t *o);

const char *cgi_get_server_protocol(cgi_t *o);
int cgi_get_server_port(cgi_t *o);
const char *cgi_get_request_method(cgi_t *o);
const char *cgi_get_path_info(cgi_t *o);
const char *cgi_get_path_translated(cgi_t *o);
const char *cgi_get_script_name(cgi_t *o);
const char *cgi_get_query_string(cgi_t *o);
const char *cgi_get_remote_host(cgi_t *o);
const char *cgi_get_remote_ident(cgi_t *o);
const char *cgi_get_content_type(cgi_t *o);
const char *cgi_get_decoded_url(cgi_t *o);
int cgi_get_content_length(cgi_t *o);
int http_send_file(char * filename);


int cgi_get_param_by_name(cgi_t *o, 
      const char *name, char *value, size_t value_len);

int cgi_get_param_by_index(cgi_t *o, int index, 
      char *name, size_t name_len, char *value, size_t value_len);

void cgi_http_header_begin(const char *content_type);
void cgi_http_header_end();
void cgi_http_header_set_cookie(char *name, char *value);
void cgi_get_cookie(const char *name, char *cookie, size_t cookie_len);




#endif




