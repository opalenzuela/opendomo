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



/** 
  @file cgi.c
  @brief CGI related functions
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <libgen.h>

#include "secfunc.h"
#include "cgi.h"
#include "file.h"
#include "regex/match.h"

#define MAX_URL_SIZE 8096


/// Convert a hexadecimal pair of digits to their decimal value 
#define BASE16_TO_10(x) (((x) >= '0' && (x) <= '9') ? ((x) - '0') : \
      (toupper((x)) - 'A' + 10))

/// Returns 1 if the character "c" is alphanumerical
int cgi_isxdigit(char c)
{
   if(isdigit(c))
      return 1;

   if('A'>=c && c<='F')
      return 1;

   if('a'>=c && c<='f')
      return 1;

   return 0;
}

// {{{ cgi_decode_url_alloc()
/// Decodes an URL
char *cgi_decode_url_alloc(char *url, size_t *nbytes) 
{
   char *out;
   char *ptr;
   const char *c;

   if (!(out = ptr = strdup(url))) 
      return 0;

   for (c=url; *c; c++) 
   {
      if (*c != '%' || !cgi_isxdigit(c[1]) || !cgi_isxdigit(c[2])) 
      {
         *ptr++ = *c;
      }
      else 
      {
         *ptr++ = (BASE16_TO_10(c[1]) * 16) + (BASE16_TO_10(c[2]));
         c += 2;
      }
   }

   *ptr = 0;
   
   if (nbytes) 
      *nbytes = (ptr - out); /* does not include null byte */

   return out;
}
// }}}

// {{{ __decode_url()
/// Decodes an URL
void __decode_url(string_t *url) 
{
   const char *c;
   char *ptr = url->str;

   for (c=url->str; *c; c++) 
   {
      if (*c != '%' || !cgi_isxdigit(c[1]) || !cgi_isxdigit(c[2])) 
      {
         *ptr++ = *c;
      }
      else 
      {
         *ptr++ = (BASE16_TO_10(c[1]) * 16) + (BASE16_TO_10(c[2]));
         c += 2;
      }
   }

	*ptr=0;
}
// }}}


// {{{ cgi_alloc()
/// Configures the CGI behaviour depending on the environment vars
cgi_t *cgi_alloc()
{
	cgi_t * o = malloc(sizeof(cgi_t));
	if(!o)
	{
		perror("malloc()");
		exit(EXIT_FAILURE);
	}


   char *p;
 
   /* SERVER_PORT */
   if( (p=getenv("SERVER_PORT")) != NULL)
      o->server_port = atoi(p);
   else
      o->server_port = 0;
   
	/* CONTENT_LENGTH */
   if( (p=getenv("CONTENT_LENGTH")) != NULL)
      o->content_length = atoi(p);
   else
      o->content_length = 0;


   /* SERVER_PROTOCOL */
   o->server_protocol = string_alloc(getenv("SERVER_PROTOCOL"));
 
 	/* REQUEST_METHOD */
   o->request_method = string_alloc(getenv("REQUEST_METHOD"));

   /* PATH_INFO */
   o->path_info = string_alloc(getenv("PATH_INFO"));

   /* PATH_TRANSLATED */
   o->path_translated = string_alloc(getenv("PATH_TRANSLATED"));

   /* SCRIPT_NAME */
   o->script_name = string_alloc(getenv("SCRIPT_NAME"));

   /* REMOTE_HOST */
   o->remote_host = string_alloc(getenv("REMOTE_HOST"));

   /* REMOTE_ADDR */
   o->remote_addr = string_alloc(getenv("REMOTE_ADDR"));

   /* AUTH_TYPE */
   o->auth_type = string_alloc(getenv("AUTH_TYPE"));

   /* REMOTE_USER */
   o->remote_user = string_alloc(getenv("REMOTE_USER"));

   /* REMOTE_IDENT */
   o->remote_ident = string_alloc(getenv("REMOTE_IDENT"));

   /* CONTENT_TYPE */
   o->content_type = string_alloc(getenv("CONTENT_TYPE"));

   /* QUERY_STRING */
   o->query_string = string_alloc(getenv("QUERY_STRING"));


	o->decoded_url = string_alloc("");

   /* POST PARAMS */
   if(o->content_length>0)
   {
		int i=0;
      size_t len = o->content_length;
      char buffer[64];
      while((fgets(buffer, sizeof(buffer), stdin)))
			string_append(o->decoded_url, buffer);
	
		syslog(LOG_DEBUG, "POST: %s\n", o->decoded_url->str);		
   }

   /* GET PARAMS */
   if(o->query_string->size>0)
   {
		if(o->decoded_url->size>1)
			string_append(o->decoded_url, "&");
	
		string_append(o->decoded_url, o->query_string->str);
		syslog(LOG_DEBUG, "GET: %s\n", o->query_string->str);		
   }
	__decode_url(o->decoded_url);

	/* remove dangerous chars */
	string_replace(o->decoded_url, "\"","");
	string_replace(o->decoded_url, "'", "");
	string_replace(o->decoded_url, ";", "");
	string_replace(o->decoded_url, "<", "");
	string_replace(o->decoded_url, ">", "");
	string_replace(o->decoded_url, "&&", "");
	string_replace(o->decoded_url, "||", "");
	


	return o;
}
// }}}

// {{{ cgi_free()
void cgi_free(cgi_t *o)
{
	if(!o)
		return;

	string_free(o->server_protocol);
   string_free(o->request_method);
   string_free(o->path_info);
   string_free(o->path_translated);
   string_free(o->script_name);
   string_free(o->query_string);
   string_free(o->remote_host);
   string_free(o->remote_addr);
   string_free(o->auth_type);
   string_free(o->remote_user);
   string_free(o->remote_ident);
   string_free(o->content_type);
   string_free(o->decoded_url);
}
// }}}


// {{{ cgi_get_server_protocol()
/// Wrapper function for the server protocol
const char *cgi_get_server_protocol(cgi_t *o)
{
   return o->server_protocol->str;
}
// }}}

// {{{ cgi_get_server_port()
/// Wrapper function for the server port
int cgi_get_server_port(cgi_t *o)
{
   return o->server_port;
}
// }}}

// {{{ cgi_get_request_method()
/// Wrapper function for the request method
const char *cgi_get_request_method(cgi_t *o)
{
   return o->request_method->str;
}
// }}}

// {{{ cgi_get_path_info()
/// Wrapper function for the path
const char *cgi_get_path_info(cgi_t *o)
{
   return o->path_info->str;
}
// }}}

// {{{ cgi_get_path_translated()
/// Wrapper function for the path translated
const char *cgi_get_path_translated(cgi_t *o)
{
   return o->path_translated->str;
}
// }}}

// {{{ cgi_get_script_name()
/// Wrapper function for the script name
const char *cgi_get_script_name(cgi_t *o)
{
   return o->script_name->str;
}
// }}}

// {{{ cgi_get_query_string()
/// Wrapper function for the query string
const char *cgi_get_query_string(cgi_t *o)
{
   return o->query_string->str;
}
// }}}

// {{{ cgi_get_remote_host()
/// Wrapper function for the remote host
const char *cgi_get_remote_host(cgi_t *o)
{
   return o->remote_host->str;
}
// }}}

// {{{ cgi_get_remote_ident()
/// Wrapper function for the remote ID
const char *cgi_get_remote_ident(cgi_t *o)
{
   return o->remote_ident->str;
}
// }}}

// {{{ cgi_get_content_type()
/// Wrapper function for the content type
const char *cgi_get_content_type(cgi_t *o)
{
   return o->content_type->str;
}
// }}}

// {{{ cgi_get_decoded_url()
/// Wrapper function for the decoded URL
const char *cgi_get_decoded_url(cgi_t *o)
{
   return o->decoded_url->str;
}
// }}}

// {{{ cgi_get_content_length()
/// Wrapper function for the content length
int cgi_get_content_length(cgi_t *o)
{
   return o->content_length;
}
// }}}

// {{{ cgi_get_param_by_name()
/// Obtain the param value by its name
int cgi_get_param_by_name(cgi_t *o, 
      const char *name, char *value, size_t value_len)
{
   char *p, *p_ini;
   int i=0;

   p = strstr(o->decoded_url->str, name);

   while(p)
   {
      if(!*(p-1))
         break;

      if(!isalpha(*(p-1)))
         break;

      p = strstr(p+1, name);
   }

   if(p) 
   {
      while ((*p!=0)&&(*p!='=')) 
         p++; 

      if(*p==0)
      {
         strcpy(value, "");
         strcpy(o->error, "param not found");
         return 0;
      }

      p++;
      p_ini = p;
      
      while((*p!=0)&&(*p!='&')) 
      { 
         p++; 
         i++; 
      } 
      if(i>value_len)
         i=value_len;

      sstrncpy(value, p_ini, i+1);
      return 1;
   }
   else
   {
      strcpy(value, "");
      strcpy(o->error, "param not found");
      return 0;
   }

}
// }}}

// {{{ cgi_get_param_by_index()
/// Obtain the param value by its index
int cgi_get_param_by_index(cgi_t *o, int index, 
      char *name, size_t name_len, char *value, size_t value_len)
{
   char *p, *p_ini;
   int i=0;

   strcpy(name, "");
   strcpy(value, "");


   if(o->decoded_url->size==0)
   {
      strcpy(o->error, "param not found");
      return 0;
   }

   p = o->decoded_url->str;

   for(i=0; i<index && p; i++)
   {
      p = strstr(p, "&");
      if(p) p++;
   }

   i=0;
   if(p) 
   {
      p_ini = p;
      
      while((*p!=0)&&(*p!='&')) 
      { 
         p++; 
         i++; 
      } 
      if(i>name_len)
         i=name_len;

      sstrncpy(name, p_ini, i+1);

      p_ini = strstr(name, "=");
      if(p_ini)
		{
         sstrncpy(value, p_ini+1, value_len);
			*p_ini = 0;
		}

      return 1;
   }

   strcpy(o->error, "param not found");
   return 0;
}
// }}}



// {{{ cgi_http_header_begin()
/// HTTP header begin
void cgi_http_header_begin(const char *content_type)
{
   //printf("HTTP/1.1 200 OK\n");
   printf("Content-type: %s\n", content_type);
}
// }}}

// {{{ cgi_http_header_end()
/// HTTP header end
void cgi_http_header_end()
{
   printf("\n");
}
// }}}

// {{{ cgi_http_header_set_cookie()
/// Send the cookie through HTTP
void cgi_http_header_set_cookie(char *name, char *value)
{
	/*
   char expires[128];
   time_t t = time(NULL)+600; // 10 min
   strftime(expires, sizeof(expires), "%a,  %d  %b  %Y  %H:%M:%S  %z", 
      (struct tm *)gmtime(&t));
   printf("Set-Cookie: %s=%s; Path=/; Expires=%s\n", name, value, expires);
	*/
   printf("Set-Cookie: %s=%s; Path=/;\n", name, value);
}
// }}}

// {{{ cgi_get_cookie()
/// Get the cookie by HTTP
void cgi_get_cookie(const char* name, char *cookie, size_t cookie_len)
{
   char *p;
   char *c;

   if((c=getenv("HTTP_COOKIE"))!=NULL)
   {
      if((p=strstr(c, name)))
      {
         p = strstr(p, "=");
         p++;
         if(p)
         {
            sstrncpy(cookie, p, cookie_len);
            p = strstr(cookie, ";");
            if(p) *p=0;
         }
      }
   }
	else
	{
		syslog(LOG_NOTICE, "HTTP_COOKIE not found\n");
	}
}
// }}}

/// Send the file by HTTP, including the Content-type
int http_send_file(char * filename){
	syslog(LOG_NOTICE, "Requesting file: %s\n", filename);

	FILE *fp;
	int i;
	if ((fp = fopen(filename,"rb"))) 
	{
		// Images
		if(match(filename,"jpeg$") || match(filename,"JPEG$")){
		   	  printf("Content-Type: image/jpeg\n");
		}else if(match(filename,"jpg$") || match(filename,"JPG$")){
		   	  printf("Content-Type: image/jpeg\n");
		}else if(match(filename,"gif$") || match(filename,"GIF$")){
		   	  printf("Content-Type: image/gif\n");
		}else if(match(filename,"png$") || match(filename,"PNG$")){
		   	  printf("Content-Type: image/png\n");
		// Videos
		}else if(match(filename,"mpg$") || match(filename,"MPG$")|| match(filename,"mpeg$")){
		   	  printf("Content-Type: video/mpeg\n");
		}else if(match(filename,"mp4$") || match(filename,"MP4$")){
		   	  printf("Content-Type: video/mp4\n");
		}else if(match(filename,"ogg$") || match(filename,"OGG$")){
		   	  printf("Content-Type: video/ogg\n");
		}else if(match(filename,"wmv$") || match(filename,"WMV$")){
		   	  printf("Content-Type: video/x-ms-wmv\n");
		}else if(match(filename,"webm$") || match(filename,"WEBM$")){
		   	  printf("Content-Type: video/webm\n");

		// Web pages & XML
		}else if(match(filename,"html$") || match(filename,"HTML$")){
		   	  printf("Content-Type: text/html\n");
		}else if(match(filename,"htm$") || match(filename,"HTM$")){
		   	  printf("Content-Type: text/html\n");
		}else if(match(filename,"xml$") || match(filename,"XML$")){
		   	  printf("Content-Type: text/xml\n");

		// Other
		}else {
		   	  printf("Content-Type: text/plain\n");
		}

		//printf("Content-Disposition: attachment; filename=%s\n", 
		printf("Content-Disposition: filename=%s\n", 
			basename(filename));
		printf("\n");

		do 
		{
		    i=fgetc(fp);
		    if (i!=EOF) putchar(i);
		} while (i!=EOF);
		fclose(fp);
		return 1;
	}
	else
	{
		syslog(LOG_NOTICE, "Error opening file: %s\n", filename);
		printf("Content-Type: text/plain\n\n");
		printf("FILE NOT FOUND\n");
		return -1;
	}
/*	file_set_filename(&fs, filename);

	printf("Content-type: text/plain\n\n");  
	if(file_is_file(&fs))
	{
		printf("HTTP/1.0 200 OK\n");
		if(match(filename,"jpg$") || match(filename,"jpeg$")){
		   	  printf("Content-Type: image/jpeg\n");
		}else if(match(filename,"gif$") || match(filename,"GIF$")){
		   	  printf("Content-Type: image/gif\n");
		}else if(match(filename,"PNG$") || match(filename,"PNG$")){
		   	  printf("Content-Type: image/png\n");
		} else {
		   	  printf("Content-Type: text/plain\n");
		}
		printf( "Content-Length: 993\n");
//		printf( "Connection: close\r\n");

//	  snprintf(command,sizeof(command), "/usr/bin/openssl enc -base64 -in %s",filename);
	  FILE *f = fopen(filename, "rb"); 
	  if(!f) {
	     syslog(LOG_ERR, "%s: %s\n", strerror(errno), filename);
	     return -1;
 	  }else{
 	  	byte = fgetc (f);
		do {
			fputc(byte,stdout);
			count++;
			byte = fgetc (f);
		} while (byte != EOF);
		syslog(LOG_NOTICE, "Bytes sent: %d\n", count);
   	     fflush(stdout);
 	  }
	  fclose(f);
	}
	else
	{
		printf("HTTP/1.0 404 Not found\r\n");
		syslog(LOG_ERR, "File not found: %s\n", filename);		
	}
	return 0; */
}
