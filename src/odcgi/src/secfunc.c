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
  @file secfunc.c
  @brief Security related functions
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <paths.h>
#include <grp.h>
#include <syslog.h>

#include "secfunc.h"

// {{{ sstrncpy()
/// Safe strncpy
void sstrncpy(char *dst, const char *src, size_t size)
{
   size_t len = size;
   char*  dstptr = dst;
   char*  srcptr = (char*)src;

   if (len && --len) 
      do { if(!(*dstptr++ = *srcptr++)) break; } while(--len);

   if (!len && size) *dstptr=0;
}
// }}}

// {{{ sstrncat()
/// Safe strncat
void sstrncat(char *dst, const char *src, size_t size)
{
   size_t len = size;
   size_t dstlen;
   char*  dstptr = dst;
   char*  srcptr = (char*)src;

   while(len-- && *dstptr) 
      dstptr++;
   
   dstlen = dstptr-dst;

   if(!(len=size-dstlen)) 
      return;

   while(*srcptr) 
   {
      if(len!=1) 
      {
         *dstptr++ = *srcptr;
         len--;
      }
      srcptr++;
   }
   *dstptr=0;  
}
// }}} 

// {{{ is_valid_utf8()
/// Returns 1 if is a valid UTF code
int is_valid_utf8(const unsigned char *input)
{
   const unsigned char *c = input;
   int i;

   for(c=input; *c; c += (i+1))
   {
      if(!(*c & 0x80)) i = 0;
      else if(!(*c & 0xc0) == 0x80) return 0;
      else if(!(*c & 0xe0) == 0xc0) i = 1;
      else if(!(*c & 0xf0) == 0xe0) i = 2;
      else if(!(*c & 0xf8) == 0xf0) i = 3;
      else if(!(*c & 0xfc) == 0xf8) i = 4;
      else if(!(*c & 0xfe) == 0xfc) i = 5;

      while(i-- > 0)
         if((*(c+i) & 0xc0) != 0x80) 
            return 0;
   }

   return 1;
}
// }}}


/// Open a file handler for dev/null
static int open_devnull(int fd) 
{
   FILE *f = 0;

   if (!fd) f = freopen(_PATH_DEVNULL, "rb", stdin);
   else if (fd == 1) f = freopen(_PATH_DEVNULL, "wb", stdout);
   else if (fd == 2) f = freopen(_PATH_DEVNULL, "wb", stderr);
   return (f && fileno(f) == fd);
}

// {{{ sanitize_files()
/// Sanitize files
int sanitize_files(void) 
{
   int fd, fds;
   struct stat st;

   if ((fds = getdtablesize()) == -1) 
      fds = 256;
   
   for (fd = 3;  fd < fds;  fd++) 
      close(fd);

   for (fd = 0;  fd < 3;  fd++)
      if (fstat(fd, &st) == -1 && (errno != EBADF || !open_devnull(fd))) 
         return -1;

   return 0;
}
// }}}

// {{{ sfork()
/// Safe fork
pid_t sfork()
{
   pid_t childpid;

   if( (childpid==fork())==-1)
      return -1;

   if(childpid!=0)
      return childpid;

   sanitize_files();
   
   // drop provileges
   gid_t newgid = getgid(), oldgid = getegid();
   uid_t newuid = getuid(), olduid = geteuid();
   if (!olduid) setgroups(1, &newgid);

   if(newgid != oldgid) 
      setregid(newgid, newgid);

   if(newuid != olduid) 
      setregid(newuid, newuid);


   return 0;
}
// }}}

// {{{ spopen()
/// Safe popen
spipe_t *spopen(const char *path, char *const argv[], char *const envp[])
{
   int stdin_pipe[2];
   int stdout_pipe[2];
   spipe_t *p;

   if( !(p=(spipe_t*)malloc(sizeof(spipe_t))))
      return 0;

   p->read_fd = p->write_fd = 0;
   p->child_pid = -1;

   if(pipe(stdin_pipe)==-1)
   {
      free(p);
      return 0;
   }

   if(pipe(stdout_pipe)==-1)
   {
      close(stdin_pipe[1]);
      close(stdin_pipe[0]);
      free(p);
      return 0;
   }

   if( !(p->read_fd=fdopen(stdout_pipe[0], "r")))
   {
      close(stdout_pipe[1]);
      close(stdout_pipe[0]);
      close(stdin_pipe[1]);
      close(stdin_pipe[0]);
      free(p);
      return 0;
   }

   if( !(p->write_fd=fdopen(stdin_pipe[1], "w")))
   {
      fclose(p->read_fd);
      close(stdout_pipe[1]);
      close(stdin_pipe[1]);
      close(stdin_pipe[0]);
      free(p);
      return 0;
   }

   if((p->child_pid=sfork())==-1)
   {
      fclose(p->write_fd);
      fclose(p->read_fd);
      close(stdout_pipe[1]);
      close(stdin_pipe[0]);
      free(p);
      return 0;
   }

   if(!p->child_pid)
   {
      close(stdout_pipe[0]);
      close(stdin_pipe[1]);
      
      if(stdin_pipe[0] != 0)
      {
         dup2(stdin_pipe[0], 0);
         close(stdin_pipe[0]);
      }
      if(stdout_pipe[1] != 1)
      {
         dup2(stdout_pipe[1], 1);
         close(stdout_pipe[1]);
      }
      execve(path, argv, envp);
      exit(127);
   }

   close(stdout_pipe[1]);
   close(stdin_pipe[0]);
   return p;
}
// }}}

// {{{ spclose()
/// Safe pclose
int spclose(spipe_t *p)
{
   int status;
   pid_t pid;

   if(p->child_pid!=-1)
   {
      do { pid = waitpid(p->child_pid, &status, 0); }
      while( pid==-1 && errno==EINTR);
   }

   if(p->read_fd) fclose(p->read_fd);
   if(p->write_fd) fclose(p->write_fd);
   free(p);

   if(pid!=-1 && WIFEXITED(status))
      return WEXITSTATUS(status);
   else
      return (pid==-1 ? -1 : 0);
}
// }}}


