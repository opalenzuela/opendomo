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
  @file file.c
  @brief File system related functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include "secfunc.h"
#include "file.h"

// {{{ file_set_filename
/// Set the filename to the file structure
void file_set_filename(file_t *o, const char *filename)
{
   strcpy(o->error, "");
   sstrncpy(o->filename, filename, sizeof(o->filename));
}
// }}}

// {{{ file_is_file()
/// Return true if the structure contains a regular file
int file_is_file(file_t *o)
{
   struct stat buf;

   strcpy(o->error, "");

   if(stat(o->filename, &buf)<0)
   {
      sstrncpy(o->error, strerror(errno), sizeof(o->error));
      return 0;
   }

   if(S_ISREG(buf.st_mode))
      return 1;

   return 0;
}
// }}}

// {{{ file_is_dir()
/// Return true if the structure contains a directory
int file_is_dir(file_t *o)
{
   struct stat buf;

   strcpy(o->error, "");

   if(stat(o->filename, &buf)<0)
   {
      sstrncpy(o->error, strerror(errno), sizeof(o->error));
      return 0;
   }

   if(S_ISDIR(buf.st_mode))
      return 1;

   return 0;

   return 0;
}
 // }}}

// {{{ file_http_print()
/** Print file in STDOUT
 @param o File structure
 @todo Make consistent with http_send_file
*/
void file_http_print(file_t *o)
{
   char buf[1024];
   char c; 

   strcpy(o->error, "");

   FILE*f = fopen(o->filename, "r");
   if(!f)
   {
      sstrncpy(o->error, strerror(errno), sizeof(o->error));
      return;
   }

   while(c=fgetc(f) != NULL)
   {
      if (c==0) fputc('\\',stdout);
      if(fputc(c, stdout) == EOF)
      {
         sstrncpy(o->error, strerror(errno), sizeof(o->error));
         fclose(f);
         return;
      }
   }

   if(ferror(f))
   {
      sstrncpy(o->error, strerror(errno), sizeof(o->error));
      fclose(f);
      return;
   }
}
// }}}

// {{{ file_print()
/** Print the contents of the file to STDOUT
 @param o File structure
 @todo: Make consistent with http_send_file and file_http_print
*/
void file_print(file_t *o)
{
   char buf[1024];

   strcpy(o->error, "");

   FILE*f = fopen(o->filename, "r");
   if(!f)
   {
      sstrncpy(o->error, strerror(errno), sizeof(o->error));
      return;
   }

   while(fgets(buf, 1024, f) != NULL)
   {
      if(fputs(buf, stdout) == EOF)
      {
         sstrncpy(o->error, strerror(errno), sizeof(o->error));
         fclose(f);
         return;
      }
   }

   if(ferror(f))
   {
      sstrncpy(o->error, strerror(errno), sizeof(o->error));
      fclose(f);
      return;
   }
}
// }}}

// {{{ file_get()
/// Copy the file contents to the buffer out
int file_get(file_t *o, char * out, int len)
{
	char buf[1024]="";
   FILE*f = fopen(o->filename, "r");
   if(!f)
   {
      sstrncpy(o->error, strerror(errno), sizeof(o->error));
      return -1;
   }

   while(fgets(buf, 1024, f) != NULL)
   {
	   sstrncpy(out,buf,len);
   }
   strtok(out,"\n");
   fclose(f);

   if(ferror(f))
   {
      sstrncpy(o->error, strerror(errno), sizeof(o->error));
      fclose(f);
      return -1;
   }
   return 0;
}
// }}}

// {{{ file_get_error()
/// Wrapper function for the error message
const char* file_get_error(file_t *o)
{
   return o->error;
}
// }}}

// {{{ file_error()
/// Returns 1 if the error message is not empty
int file_error(file_t *o)
{
   if(strlen(file_get_error(o))>0)
      return 1;

   return 0;
}
// }}}







