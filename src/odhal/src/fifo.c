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

/* 
 * author: Daniel Lerch <dlerch@gmail.com>, http://opendomo.org
 * date: August 2009
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "readline.h"
#include "printlog.h"
#include <unistd.h>


int is_pipe(const char *path)
{
	struct stat sb;

	if (stat(path, &sb) == -1) 
		return 0;

	if((sb.st_mode & S_IFMT)==S_IFIFO)
		return 1;

	return 0;
}

int file_exist (char *filename)
{
	struct stat   buffer;   
	return (stat (filename, &buffer) == 0);
}

// {{{ fifo_input()
pid_t fifo_input(const char *path, 
   void (*callback)(char *buff, size_t bufflen, void *udata), void *udata)
{
	int fd = 0;
   pid_t p = 0;
   char buff[64];
	
	if(is_pipe(path))
		return 0;

   /* Child process */
   if((p=fork())==0)
   {
      while(1)
      {        
         mkfifo(path, 0644);
         chmod(path, 0644);

         fd = open(path, O_WRONLY);
         if(fd<0)
         {
            printf("open() : fifo_input  %s : %s\n", path, strerror(errno));
            return -1;
         }

         callback(buff, sizeof(buff), udata);
         write(fd, buff, strlen(buff));

         close(fd);
         unlink(path);
      }

      exit(0);
   }
   
   /* Error */
   else if(p<0)
   {
      perror("fork()");
      return -1;
   }

   /* Parent */
	return p;
}
// }}}

// {{{ fifo_output()
pid_t fifo_output(const char *path, 
   void (*callback)(char *buff, size_t bufflen, void *udata), void *udata)
{
   int fd = 0;
   pid_t p = 0;
   char buff[64];

	if(is_pipe(path))
		return 0;

	printlog(LOG_N, "Create fifo: '%s'\n", path);


   /* Child process */
   if((p=fork())==0)
   {
      while(1)
      {        
  			mkfifo(path, 0722);
         chmod(path, 0722);

         fd = open(path, O_RDONLY);
         if(fd<0)
         {
            printf("open() : fifo_output  %s : %s\n", path, strerror(errno));
            return -1;
         }

         int n = readline(buff, sizeof(buff), fd);
			if(n>0)
			{
         	buff[n] = 0;
	         callback(buff, sizeof(buff), udata);
			}

         close(fd);
      	unlink(path);
      }

      exit(0);
   }
   
   /* Error */
   else if(p<0)
   {
      perror("fork()");
      return -1;
   }

   /* Parent */
	return p;
}
// }}}




