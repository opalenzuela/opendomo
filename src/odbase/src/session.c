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



#include <openssl/bn.h>
#include <openssl/rand.h>
#include <pwd.h>
#include <shadow.h>
#include <crypt.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <syslog.h>

#include "libconfig.h"
#include "secfunc.h"
#include "session.h"
#include "conf.h"
#include "util.h"

// {{{ session_gen_random()
int session_gen_random(char *rnd, size_t bytes_len)
{
   BIGNUM *p = BN_new();
   
   if(RAND_load_file("/dev/urandom", bytes_len) < bytes_len)
   {
      return -1;
   }
   
   if(!BN_rand (p, bytes_len/8, 1, 1))
   {
      return -2;
   }

   sstrncpy(rnd, BN_bn2hex(p), bytes_len);
   BN_free (p);
   return 0;
}
// }}}

// {{{ session_check_pass()
int session_check_pass(const char *clear_pass, const char *encrypted_pass) 
{
   char *encrypted = crypt(clear_pass, encrypted_pass); 

   if (strcmp(encrypted, encrypted_pass) == 0)  
      return 1;
   else 
      return 0;
}
// }}}

// {{{ session_is_valid_user()
int session_is_valid_user(const char *user, const char* pass)
{
   struct passwd *pw = getpwnam(user);
   if(pw==NULL)
      return 0;

   // password in /etc/shadow
   if(strcmp(pw->pw_passwd, "x")==0)
   {
      /* Solo root puede leer /etc/shadow por lo que esta parte no
       se funcionara a menos que la ejecute root o el archivo este
       setuidado */

      struct spwd *sp = getspnam(user);
      if(sp==NULL)
         return 0;

      if(session_check_pass(pass, sp->sp_pwdp))
         return 1;
   }

   // password in /etc/passwd
   else
   {
      if(session_check_pass(pass, pw->pw_passwd))
         return 1;
   }

   return 0;
}
// }}}

// {{{ session_set_ids()
void session_set_ids(const char *user)
{
	int r1, r2, r3, r4;
   if(strcmp(user,"")==0)
   {
      setregid(OD_NOBODY_USERID, OD_NOBODY_USERID);
      setreuid(OD_NOBODY_USERID, OD_NOBODY_USERID);
      setgid(OD_NOBODY_USERID);
      setuid(OD_NOBODY_USERID);
      return;
   }

   struct passwd *pw = getpwnam(user);
   if(pw==NULL)
   {
      setregid(OD_NOBODY_USERID, OD_NOBODY_USERID);
      setreuid(OD_NOBODY_USERID, OD_NOBODY_USERID);
      setgid(OD_NOBODY_USERID);
      setuid(OD_NOBODY_USERID);
      return;
   }
   //TODO Assign right GID
   setgroups(1, &pw->pw_gid);
   r1=setregid(pw->pw_gid, pw->pw_gid);
   r2=setreuid(pw->pw_uid, pw->pw_uid);
   r3=setgid(pw->pw_gid);
   r4=setuid(pw->pw_uid);
   
   syslog (LOG_NOTICE, "[odcgi] priv gid: %d, uid: %d\n",
	  pw->pw_gid, pw->pw_uid);
   syslog (LOG_NOTICE, "[odcgi] retvals: %d, %d, %d, %d\n",
	  r1, r2, r3, r4);	  
}
// }}}

// {{{ session_is_valid_sessid()
int session_is_valid_sessid(const char *sessid)
{
	// TODO use timestamp: session not expires.
	
   struct config_t cfg;
   config_init(&cfg);
   config_setting_t *cs;
   config_setting_t *vs;

   if(!config_read_file(&cfg, OD_SESSION_FILE))
      return 0;

   int i=0;
   for(i=0; ;i++)
   {
      if( !(cs = config_setting_get_elem(cfg.root, i)) )
         break;

      if( !(vs = config_setting_get_member(cs, "sessid")) )
         continue;

      const char *res = config_setting_get_string(vs);
      if(res)
         if(strcmp(res, sessid)==0)
         {
            config_destroy(&cfg);
            return 1;
         }

   }

   config_destroy(&cfg);
   return 0;
}
// }}}

// {{{ session_get_user()
int session_get_user(const char *sessid, char *user, size_t len)
{
   struct config_t cfg;
   config_init(&cfg);
   config_setting_t *cs;
   config_setting_t *vs;

   if(!config_read_file(&cfg, OD_SESSION_FILE))
      return -1;

   int i=0;
   for(i=0; ;i++)
   {
      if( !(cs = config_setting_get_elem(cfg.root, i)) )
         break;

      if( !(vs = config_setting_get_member(cs, "sessid")) )
         continue;

      char *session_user = config_setting_name(cs);
      if(!session_user)
         continue;

      const char *res = config_setting_get_string(vs);
      if(res)
         if(strcmp(res, sessid)==0)
         {
            sstrncpy(user, session_user, len);
            config_destroy(&cfg);
            return 0;
         }

   }

   config_destroy(&cfg);
   return -1;
}
// }}}

// {{{ session_is_alnum()
int session_str_is_alnum(const char *str)
{
   const char *p = str;
   while(*p!=0)
   {
      if(!isalnum(*p))
         return 0;
      p++;
   }
   return 1;
}
// }}}

// {{{ session_store_session()
int session_store_session(const char* user, const char* sessid)
{
	if(strlen(user)==0 || strlen(sessid)==0)
		return -1;

   char var[256];
   struct config_t cfg;
   config_init(&cfg);

   char date[255];
   util_get_date(date, sizeof(date), "%Y%m%d%H%M%S");

   config_setting_t *cs;
   config_setting_t *us;

   if(!config_read_file(&cfg, OD_SESSION_FILE))
      config_write_file(&cfg, OD_SESSION_FILE);


	if( !(us = config_lookup(&cfg, user)) )
	{
		us = config_setting_add(cfg.root, user, CONFIG_TYPE_GROUP);
		cs = config_setting_add(us, "sessid", CONFIG_TYPE_STRING);
		cs = config_setting_add(us, "timestamp", CONFIG_TYPE_STRING);
	}

	snprintf(var, sizeof(var), "%s.sessid", user);
	if( !(cs = config_lookup(&cfg, var)) )
		return 0;
	config_setting_set_string(cs, sessid);

	snprintf(var, sizeof(var), "%s.timestamp", user);
	if( !(cs = config_lookup(&cfg, var)) )
		return 0;
	config_setting_set_string(cs, date);

	config_write_file(&cfg, OD_SESSION_FILE);
	config_destroy(&cfg);
	return 0;
}
// }}}

// {{{ session_delete_session()
void session_delete_session(const char* user)
{
	if(strlen(user)==0)
		return;

   struct config_t cfg;
   config_init(&cfg);

   config_setting_t *us;

   if(!config_read_file(&cfg, OD_SESSION_FILE))
      config_write_file(&cfg, OD_SESSION_FILE);

   if( !(us = config_lookup(&cfg, user)) )
      return;

   us = config_setting_remove(cfg.root, user);

   config_write_file(&cfg, OD_SESSION_FILE);
   config_destroy(&cfg);
}
// }}}


