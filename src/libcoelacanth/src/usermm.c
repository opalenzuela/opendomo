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


#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pwd.h>
#include<shadow.h>

#include "unix/user.h"

void usage(const char *progname)
{
	printf(
		"Usage: %s [admin passwd] set <user> <pass>\n"
		"                         add <user> <pass>\n"
		"                         rm  <user>\n"
		, progname);

}

int main(int argc, char *argv[])
{
	if(argc<4)
	{
		usage(argv[0]);
		return 0;
	}

	char *user = "admin";
	char *pass = argv[1];

	// ------------------------------------------------------------------------
	// Only admin user can use this tool. Check user.
	// ------------------------------------------------------------------------
   struct passwd *pw = getpwnam(user);
   if(pw==NULL)
	{
		fprintf(stdout, "admin user not found\n");
      return -1;
	}

   // password in /etc/shadow
   if(strcmp(pw->pw_passwd, "x")==0)
   {
      struct spwd *sp = getspnam(user);
      if(sp==NULL)
		{
			fprintf(stdout, "Program must be setuid root\n");
      	return -1;
		}

   	char *encrypted = crypt(pass, sp->sp_pwdp); 
   	if(strcmp(encrypted, sp->sp_pwdp) != 0)
		{
			fprintf(stdout, "Incorrect password for admin\n");
      	return -1;
		}
			
   }

   // password in /etc/passwd
   else
   {
   	char *encrypted = crypt(pass, pw->pw_passwd); 
   	if(strcmp(encrypted, pw->pw_passwd) != 0)
		{
			fprintf(stdout, "Incorrect password for admin\n");
      	return -1;
		}
   }



	// ------------------------------------------------------------------------
	// Process params
	// ------------------------------------------------------------------------
	if(strcmp(argv[2], "rm")==0 && argc==4)
	{
		user_t u;
		user_del(&u, argv[3]);
	}
	else if(strcmp(argv[2], "set")==0 && argc==5)
	{
		if(strcmp(argv[3], "root")==0)
		{
			fprintf(stdout, "root password can not be changed.\n");
			exit(-1);
		}

		user_t u;
		user_set_password(&u, argv[3], argv[4]);
		if(user_has_error(&u))
		{
			fprintf(stdout, "%s\n", user_get_error(&u));
			exit(-1);
		}
		exit(0);

	}
	else if(strcmp(argv[2], "add")==0 && argc==5)
	{
		user_t u;
		user_add(&u, argv[3], argv[4]);

		if(user_has_error(&u))
		{
			fprintf(stdout, "%s\n", user_get_error(&u));
			exit(-1);
		}
		exit(0);
	}
	else
	{
		usage(argv[0]);
	}

	return 0;

}

