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
  @file user.c
  @brief User managing functions
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <shadow.h>
#include <sys/time.h>
#include <errno.h>

#include "secfunc.h"
#include "user.h"

/// Seconds in a day
#define DAY (24L*3600L)
/// Seconds in a week
#define WEEK (7*DAY)
/// Time scale used
#define SCALE DAY
/// External function for encrypting the user's password
extern char *crypt(const char *key, const char *salt);


// {{{ user_del_line()
/// Delete the user's line in the specified file (passwd, shadow)
int user_del_line(const char *username, const char* filename)
{
	char *buffer;
	FILE *passwd;
	size_t len;
	int begin;
	int end;
	char needle[256];
	char *start;
	char *stop;
	struct stat statbuf;

	passwd = fopen(filename, "r");
	if (!passwd) 
		return -1;
	
	stat(filename, &statbuf);
	len = statbuf.st_size;
	buffer = (char *) malloc(len * sizeof(char));

	if (!buffer) 
	{
		fclose(passwd);
		return -1;
	}
	
   fread(buffer, len, sizeof(char), passwd);

	fclose(passwd);

	snprintf(needle, 256, "\n%s:", username);
	needle[255] = 0;
	start = strstr(buffer, needle);

	if (!start) 
	{
		begin = 0;
		end = 0;
	}
	else
	{
		start++;
		stop = strchr(start, '\n');
   		begin = start - buffer;
   		end = stop - buffer;
	}

	passwd = fopen(filename, "w");
	if (!passwd) 
		return -1;
	
	fwrite(buffer, (begin - 1), sizeof(char), passwd);
	fwrite(&buffer[end], (len - end), sizeof(char), passwd);

	fclose(passwd);
	return 0;
}
// }}}

// {{{ user_add()
/// Create a valid user account
void user_add(user_t *o, char *username, volatile char *passwd)
{
   o->error[0]=0;

   struct passwd p;
   struct passwd *pw;
   struct spwd sp;
   FILE *f; 
   int min = 1000;
   int max = 65000;
   char home[256];

   snprintf(home, sizeof(home), "/home/%s", username);

   p.pw_name = (char *)username;
   p.pw_passwd = "x";
   p.pw_uid = USER_DEFAULT_ID;
   p.pw_gid = USER_GROUP_ID;
   p.pw_gecos = "OpenDomo User";
   p.pw_dir = home;
   p.pw_shell = "/bin/sh";


   f = fopen("/etc/passwd", "r");

   /* check user and get valid id */
   while ((pw = fgetpwent(f))) 
   {
      if (strcmp(pw->pw_name, p.pw_name) == 0) 
      {
         sstrncpy(o->error, "user_add(): user exists", USER_ERROR_SIZE);
         return;
      }

      if ((pw->pw_uid >= p.pw_uid) && (pw->pw_uid < max)
            && (pw->pw_uid >= min)) 
      {
         p.pw_uid = pw->pw_uid + 1;
      }
   }

   fclose(f);

   f = fopen("/etc/passwd", "a+");
   if(!f)
   {
      sstrncpy(o->error, "user_add(): cannot open /etc/passwd",USER_ERROR_SIZE);
      return;
   }


   /* add to passwd */
   if (putpwent(&p, f) == -1) 
   {
      sstrncpy(o->error, "user_add(): putpwent() error", USER_ERROR_SIZE);
      return;
   }

   fclose(f);


   /* salt */
   struct timeval tv;
   static char salt[40];

   salt[0] = '\0';

   gettimeofday (&tv, (struct timezone *) 0);
   strcat(salt, l64a (tv.tv_usec));
   strcat(salt, l64a (tv.tv_sec + getpid () + clock ()));

   if (strlen (salt) > 3 + 8)  
      salt[11] = '\0';


   /* shadow */
   sp.sp_namp = p.pw_name;
   sp.sp_pwdp = (char*)crypt((const char*)passwd, salt);
   sp.sp_min = 0;
   sp.sp_max = (10000L * DAY) / SCALE;
   sp.sp_lstchg = time((time_t *) 0) / SCALE;
   sp.sp_warn = -1;
   sp.sp_expire = -1;
   sp.sp_inact = -1;
   sp.sp_flag = -1;


   /* add to shadow */
   f = fopen("/etc/shadow", "a+");
   if(!f)
   {
      sstrncpy(o->error, "user_add(): cannot open /etc/shadow",USER_ERROR_SIZE);
      return;
   }

   if (putspent(&sp, f) == -1) 
   {
      sstrncpy(o->error, "user_add(): putspent() error",USER_ERROR_SIZE);
      return;
   }

   fclose(f);

   /* Create home */
   mkdir(home, 0700);  
   chown(home, p.pw_uid, USER_GROUP_ID);
}
// }}}

// {{{ user_del()
/** Delete specified user
 @param o User structure
 @param username User name to be deleted
 */
void user_del(user_t *o, char *username)
{
   /// @todo: warning! final state may be inconsistent

   o->error[0]=0;

   if(user_del_line(username, "/etc/passwd")!=0)
   {
      sstrncpy(o->error, "user_del() can not remove user from /etc/passwd",
         USER_ERROR_SIZE);
      return;
   }


   if(user_del_line(username, "/etc/shadow")!=0)
   {
      sstrncpy(o->error, "user_del() can not remove user from /etc/passwd",
         USER_ERROR_SIZE);
      return;
   }

}
// }}}

// {{{ user_set_password()
/// Set the password for the specified username
void user_set_password(user_t *o, char *username, volatile char* passwd)
{
   FILE *f;
   struct spwd *sp = NULL;

   o->error[0]=0;

   sp = getspnam(username);
   if(!sp)
   {
      sstrncpy(o->error, "user_set_password() unknown user",
         USER_ERROR_SIZE);
      return;
   }


   /* salt */
   struct timeval tv;
   static char salt[40];

   salt[0] = '\0';

   gettimeofday (&tv, (struct timezone *) 0);
   strcat(salt, l64a (tv.tv_usec));
   strcat(salt, l64a (tv.tv_sec + getpid () + clock ()));

   if (strlen (salt) > 3 + 8)  
      salt[11] = '\0';


   /* shadow */
   sp->sp_pwdp = (char*)crypt((const char*)passwd, salt);


   if(user_del_line(username, "/etc/shadow")!=0)
   {
      sstrncpy(o->error, "user_set_password() cannot modify /etc/shadow",
         USER_ERROR_SIZE);
      return;
   }

   f = fopen("/etc/shadow", "a+");
   if(!f)
   {
      sstrncpy(o->error, "user_set_password(): cannot open /etc/shadow",
            USER_ERROR_SIZE);
      return;
   }

   if (putspent(sp, f) == -1) 
   {
      sstrncpy(o->error, "user_add(): putspent() error", USER_ERROR_SIZE);
      return;
   }

   fclose(f);



}
// }}}



