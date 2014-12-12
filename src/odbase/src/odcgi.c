/*****************************************************************************
 *  This file is part of the OpenDomo project.
 *  Copyright(C) 2014 OpenDomo Services SL
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
  @file odcgi.c
  @brief ODCGI main file
  @mainpage Web Interface
	This module is the main entry point to OpenDomo
 */

#define TRUE 1
#define FALSE -1


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <libgen.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <linux/if_ether.h>
#include <sys/ioctl.h>
#include <src/match.h>
#include <stdarg.h>

#include "conf.h"
#include "cgi.h"
#include "session.h"
#include "file.h"
#include "util.h"
#include "secfunc.h"
#include "user.h"
#include "script.h"
#include "i18n.h"
//#include "mjpg.h"
//#include "odcgi_mjpg.h"
#include "src/string.h"
#include "src/str.h"


/*! \brief DOCTYPE Does not work until CSS3+HTML5 */
#define W3C_DOCTYPE  "<!DOCTYPE html> \n"

//! HTML Copyright fragment
#define ODCOPYRIGHT "<a target='opendomoORG' href='http://es.opendomo.org'>OpenDomo &copy; 2006-2014</a>"

#define ODCGI_PRIVILEGED_USER	"admin"	//!< privileged username
#define ODCGI_SESSION_NAME    "HTSESSID"	//!< Session cookie name

#define ODCGI_MSG  "Welcome to OpenDomo"	//!<  Welcome text
#define ODCGI_ERROR__RANDOM   "Random generator problem"	//!< errstring random
#define ODCGI_ERROR__PARAMS   "Incorrect params"	//!< errstring params
#define ODCGI_ERROR__SESSION   "Issue55, please notify it to developers"	//!< errstring session
/// Error message for Root has no access
#define ODCGI_ERROR__ROOT_NO_ACCESS   "root has not access"
/// Error message for Access denied
#define ODCGI_ERROR__USER_ACCESS_DENIED   \
		  "Access Denied, invalid user or password"
/// Error message for Session denied
#define ODCGI_ERROR__SESSION_ACCESS_DENIED   "Access Denied, invalid session"
/// Error message for unknown user
#define ODCGI_ERROR__UNKNOWN_USER   "Unknown User/session"
/// Error message for User management routines
#define ODCGI_ERROR__USER_MGMT   "User managment"
/// Error message for Invalid path
#define ODCGI_ERROR__INVALID_PATH   "Invalid PATH variable"
/// Error message for Cannot access root dir
#define ODCGI_ERROR__ROOT_PATH_ACCESS   "Unable to access /root directory"
/// Error message for Script not found
#define ODCGI_ERROR__SCRIPT_NOT_FOUND "Requested script was not found"

#define ODCGI_ERROR__CONNECTION_FAILURE "Connection failure"
#define ODCGI_TEXT_BACK 	"Back"	//!< Back button caption
#define ODCGI_TEXT_LOGOUT 	"Logout"	//!< Logout button caption
#define ODCGI_TEXT_DEBUG 	"Debug"	//!< Debug button caption
#define ODCGI_TEXT_SAVE  	"Save"	//!< Debug button caption

#define BUTTON_NONE 0		///< Add nothing
#define BUTTON_LOGOUT 4		///< Add a Logout button
#define BUTTON_DEBUG 8		///< Add a Debug button

/// Format for the XML message
#define MSG_XML_FORMAT "<message title=\"%s\" description=\"%s\"/>\n"

/// Back button
#define HTML_BACK_LINK "<button onclick='history.back();'>"ODCGI_TEXT_BACK"</button>"

/// Message for noscript browsers
#define ERROR__NOSCRIPT \
		  "You must have scripts enabled to access this features"

/// Configuration file where the selected theme is stored
//#define OD_APP_STYLE_CONF "/etc/opendomo/cgi_style"
/// Configuration file where the selected skin is stored
//#define OD_APP_SKIN_CONF "/etc/opendomo/cgi_skin"

/// OpenDomo version file (deprecated in 2.1, use about.sh instead)
//#define ODVERSION_FILE "/etc/VERSION"
/// Read a file and stores the content in "var"
#define read_config_file(fh,fname,var,default) if(!(fh=fopen(fname, "r"))) \
   {\
      strncpy(var, default, sizeof(var)); \
   }\
   else  {\
      fscanf(fh, "%s", var);\
      fclose(fh);    \
   }

/* globals */
//char style[100]="default";    ///< Default theme
//char skin[100]="silver";      ///< Default skin
char lang[8] = "en";		///< Default language
//! Expected Content-type
enum
{
  html = 1,
  xml = 2,
  other = 4,
  none = 0
} gui = html;

/// i18n translation function 
#define T(text) i18n(text, lang, TRUE)
/// i18n CLEAN translation function (verbose=FALSE)
#define CT(text) i18n(text, lang, FALSE)


/*! Send an event to the event manager
	\param level Valid level (warn, error, notice, ...)
	\param format String format
	\return 0 if correct, -1 if some error is produced
 */
int
logevent (char *level, const char *format, ...)
{
  va_list l;
  FILE *fp;
  char cmd[1024];
  char msg[512];

  va_start (l, format);
  vsnprintf (msg, sizeof (msg), format, l);
  va_end (l);

  //! @todo possible code injection security bug
  snprintf (cmd, sizeof (cmd), "test -x /bin/logevent && "
	    "/bin/logevent %s odcgi '%s'", level, msg);

  fp = popen (cmd, "r");
  if (fp != NULL)
    {
      pclose (fp);
    }
  else
    {
      return -1;
    }
  return 0;
}

// }}}

// {{{ odcgi_print_header()
/// Print header in HTML or XML
void
odcgi_print_header (char *scriptname, const char *username)
{
  if (gui == xml)
	{
		printf ("<?xml version='1.0' encoding='UTF-8' ?>\n" "<odcgi>\n");
	}
  else
    {
      printf (W3C_DOCTYPE
		"<html manifest='/manifest.appcache' lang='%s'>\n"
		"   <head>\n"
		"\t<meta name='apple-mobile-web-app-capable' content='yes'/>\n"
		"\t<meta http-equiv='X-UA-Compatible' content='IE=Edge'/>\n"
		"\t<link rel='apple-touch-icon' href='/apple-touch-icon.png'/>\n"
		"\t<meta http-equiv='content-type' content='text/html; charset=utf-8'>\n"
		"\t<meta name='viewport' content='width=device-width, maximum-scale=1.0, maximum-scale=1.0, user-scalable=0' />\n"
		"\t<meta name='msapplication-config' content='/IEconfig.xml' />\n"
		"\t<link rel='shortcut icon' href='/apple-touch-icon.png' type='image/x-icon'/>\n"
		"\t<link rel='stylesheet' type='text/css' href='/cgi-bin/css.cgi?%s'/>\n"
		"\t<link rel='stylesheet' type='text/css' href='/css/%s.css'/>\n"
		"\t<script type='text/JavaScript' src='/scripts/vendor/jquery.min.js'></script>\n"
		//"\t<script type='text/JavaScript' src='/scripts/vendor/jquery.flot.min.js'></script>\n"
		//"\t<script type='text/JavaScript' src='/scripts/vendor/jquery.flot.pie.min.js'></script>\n"
		//"\t<!--[if IE]><script language='javascript' src='/scripts/excanvas.js' type='text/javascript'></script><![endif]-->\n"
		//"\t<script type='text/javascript' src='/scripts/tabber.js'></script>\n"
		//"\t<script type='text/javascript' src='/scripts/vendor/datepicker/js/datepicker.js'></script>\n"
		//"\t<link href='/scripts/datepicker/css/datepicker.css' rel='stylesheet' type='text/css' />\n"
		//"\t<script type='text/javascript' src='/scripts/vendor/jquery.mousewheel.js'></script>\n"
		//"\t<script type='text/javascript' src='/scripts/vendor/jquery.jscrollpane.min.js'></script>\n"
		"\t<script type='text/JavaScript' src='/cgi-bin/js.cgi'></script>\n"
		"\t<script type='text/JavaScript' src='/scripts/%s.js'></script>\n"
		//"\t<script type='text/JavaScript' src='/cgi-bin/%s'></script>\n"	
		"\t<title>OpenDomoOS</title>\n"		
		"   </head>\n"
		"   <body>\n"
		"   <div id='logo_od'><img src='/images/default/logo.png'/>\n"
		"\t<a href='/cgi-bin/od.cgi'>Home</a></div>\n",
		lang, username, scriptname, scriptname);
    }

  char theme[16] = "unknown";

  // print theme info
  FILE *f = fopen ("/etc/opendomo/cgi_style", "r");
  if (f)
    {
		fgets (theme, sizeof (theme), f);
		int l = strlen (theme);
		if (l < sizeof (theme) && l > 0 && !isprint (theme[l - 1]))
			theme[l - 1] = 0;

		fclose (f);
    }

 /* char *ua = getenv ("HTTP_USER_AGENT");
  if (ua)
    {
      if (strstr (ua, "iPhone") ||
	  strstr (ua, "iPod") ||
	  strstr (ua, "iPad") || strstr (ua, "Android"))
	strcpy (theme, "iphone");
    }
*/
  printf ("<input type='hidden' id='theme-active' value='%s'/>\n", theme);
}

// }}}

// {{{ odcgi_get_footer()
/// Print footer in HTML or XML
void
odcgi_print_footer (const char *msg, int buttons, cgi_t * cgi)
{
	FILE *fh;
	char value[10] = "Undefined";
	if (gui == xml)
	{
		printf ("   <message description=\"%s\"/>\n</odcgi>", msg);
	}
	else
	{
		printf ("\n<div id='footer'>\n\n");
		/* Moved to header in version 2.2
		printf
			("\t\t<a id='logoutbutton' class='tool' "
			"href='?LOGOUT=1'>%s</a>\n", T (ODCGI_TEXT_LOGOUT));
		//printf ("\t\t<a id='dbgbutton' class='dbg_button' href='#' "
		//	"onclick='showDebug();'>%s</a>\n", T (ODCGI_TEXT_DEBUG));
		if (1000 == getgid()) {
			printf ("\t\t<a id='savebutton' class='tool' "
				"href='/cgi-bin/od.cgi/manageConfiguration.sh' >%s</a>\n", 
				T (ODCGI_TEXT_SAVE));			
		}
		*/
	
	// Debug block
	//read_config_file (fh, ODVERSION_FILE, value, "Unknown");
	printf ("\n<div id='debug_box' class='debug'>\n"
	 "<h2>Debug Info:</h2>\n<pre name='debuginfo'>\n");
	printf ("\tODCGI BUILD: " __DATE__ " " __TIME__ "\n");
	//printf ("\tOD VERSION: %s\n", value);
	printf ("\tUID: %d \n", getuid ());
	printf ("\tGID: %d \n", getgid ());
	printf ("\tCGI_SERVER_PROTOCOL: %s\n", cgi_get_server_protocol (cgi));
	printf ("\tCGI_SERVER_PORT: %d\n", cgi_get_server_port (cgi));
	printf ("\tCGI_REQUEST_METHOD: %s\n", cgi_get_request_method (cgi));
	printf ("\tCGI_PATH_INFO: %s\n", cgi_get_path_info (cgi));
	printf ("\tCGI_PATH_TRANSLATED: %s\n", cgi_get_path_translated (cgi));
	printf ("\tCGI_SCRIPT_NAME: %s\n", cgi_get_script_name (cgi));
	printf ("\tCGI_QUERY_STRING: %s\n", cgi_get_query_string (cgi));
	printf ("\tCGI_REMOTE_HOST: %s\n", cgi_get_remote_host (cgi));
	printf ("\tCGI_REMOTE_IDENT: %s\n", cgi_get_remote_ident (cgi));
	printf ("\tCGI_CONTENT_TYPE: %s\n", cgi_get_content_type (cgi));
	printf ("\tCGI_CONTENT_LENGTH: %d\n", cgi_get_content_length (cgi));
	printf ("\tCGI_DECODED_URL: %s\n", cgi_get_decoded_url (cgi));
	printf ("\n</pre>\n");
	//printf ("\n<button onclick='notifyProblem();'>Notify</button>");
	//printf ("\n<button onclick='putFlags();'>Translate</button>");
	printf ("\n</div>\n");

	printf ("      <p class='copyright'>" ODCOPYRIGHT " </p>\n"
		"      </div>\n"
		"   </body>\n" "</html>\n");
    }
//   return foo;
}

// }}}


// {{{ odcgi_print_login_form()
/** Print login form
 @param message Welcome or error message
 @param username Preselected user name
 */
void
odcgi_print_login_form (const char *message, const char *username)
{
  char username_label[64] = "";
  char password_label[64] = "";
  char message_label[100] = "";
  cgi_t *cgi = cgi_alloc ();

  sstrncpy (username_label, CT ("Username"), sizeof (username_label));
  sstrncpy (password_label, CT ("Password"), sizeof (password_label));
  sstrncpy (message_label, CT (message), sizeof (message_label));

  if (gui == xml)
    {
      if (strlen (message) == 0)
	sstrncpy (message_label, T (ODCGI_MSG), sizeof (message_label));
      cgi_http_header_begin ("text/xml");
      cgi_http_header_end ();
      odcgi_print_header ("loginform", username);
      printf ("  <gui>\n");
      // Usando la API, nunca debería mostrarse al usuario esta salida
      printf ("     <error>%s</error>\n", message_label);
      printf ("     <text id='user' label=\"%s:\"/>\n"
	      "     <password id='pass' label=\"%s:\"/>\n"
	      "     <actions>\n"
	      "        <action name='default' url='" OD_URI "/'/>\n"
	      "     </actions>\n"
	      "  </gui>\n", username_label, password_label);

    }
  else
    {
      if (strlen (message) == 0)	// Si no hay ningún mensaje, damos la bienvenida
	sstrncpy (message_label, T (ODCGI_MSG), sizeof (message_label));
      //! @todo adaptar "action" para obtener la ruta actual
      cgi_http_header_begin ("text/html");
      cgi_http_header_end ();
      odcgi_print_header ("/loginform", username);
      printf ("<div id='header'></div>\n"
	      "<div id='main'>\n"
	      "<form id='loginform_frm' action='" OD_URI "/control' method='post'>\n"
	      "<fieldset class='tabform'>\n"
	      " <h1>%s</h1>\n"
	      " <ul id='login' class='loginform'>\n"
	      "  <li id='USER_lbl' class='input'><label for='USER'>%s:</label>"
	      "<p><input type='text' id='USER' name='USER' value=\"%s\"  autocapitalize='off'/></p></li>\n"
	      "  <li id='PASS_lbl' class='password'><label for='PASS'>%s:</label>"
	      "<p><input type='password' id='PASS' name='PASS'/></p></li>\n"
	      " </ul>\n"
	      " <div id='advpad'></div>\n",
	      message_label, username_label, username, password_label);
      printf (" <div class='toolbar' id='login_tbr'>\n");
//      printf ("  <a href='/wzconf/' target='_top' class='buttonlnk'>%s</a>",
//	      T ("Configuration wizards"));
      printf ("  <button type='button' onclick=\"help('Login');\">%s</button>",
      	T ("Help"));
      printf ("  <button type='submit'>%s</button>\n", T ("Login"));
      printf (" </div>\n  </fieldset>\n </form>\n</div>\n");
    }
  odcgi_print_footer ("", BUTTON_NONE, cgi);
  cgi_free (cgi);
  //fflush(stdout);
}

// }}}

// {{{ odcgi_msg()
/// Format a message in HTML or XML
void
odcgi_msg_error (const char *title, const char *description)
{
  char tit[64];
  sstrncpy (tit, title, sizeof (tit));

  if (gui == xml)
    {
      printf (MSG_XML_FORMAT, tit, T (description));
    }
  else
    {
     printf (
		  "<div id='main'>\n"
		  " <fieldset class='error'>\n"
		  "  <legend id='error_lgnd'>%s</legend>\n",T("Error"));
	printf("  <h1>%s</h1>\n", T(tit));
	printf("   <p>%s</p>\n",  T(description));
	printf("  <div class='toolbar'>");
	printf("  <button type='button' onclick='history.back();'>%s</button>", 
		T("Back"));
	printf("  <button type='button' onclick=\"help('%s');\">%s</button>", 
		title, T("Help"));
	printf("  </div>");
	printf(" </fieldset>");
	printf("</div>");	
    }
}
// }}}

// {{{ odcgi_login()
/** Start a new session
 @param env Environment variables
 @param http_user User name sent by HTTP
 @param http_pass Password sent by HTTP
 @param http_session Session ID
 @param len Lenght of the variables
*/
	int
odcgi_login (script_env_t * env, const char *http_user,
		const char *http_pass, char *http_session, size_t len)
{
	/* Login by user/pass */
	if ((strlen (http_user) > 0) && (strlen (http_pass) > 0))
	{
		if (!session_is_valid_user (http_user, http_pass))
		{
			odcgi_print_login_form (ODCGI_ERROR__USER_ACCESS_DENIED, http_user);
			logevent ("warning", "Login failure for [%s]", http_user);
			return -1;
		}

		/* Create random session id */
		if (session_gen_random (http_session, len) == 0)
		{
			if (session_store_session (http_user, http_session) != 0)
				syslog (LOG_WARNING, "session not saved: %s\n", http_session);
		}
		else
		{
			odcgi_msg_error (ODCGI_ERROR__RANDOM, 
					"Cannot generate random code to identify this session.");
			return -1;
		}
		logevent ("notice", "Login user [%s] ", http_user);
		sstrncpy (env->user, http_user, MAX_ENV_SIZE);
	}

	/* Login by session id */
	else
	{
		/* Is valid session? */
		if (strlen (http_session) > 2)
		{
			if (!session_is_valid_sessid (http_session))
			{
				/* Maybe old cookie: no error */
				odcgi_print_login_form ("", http_user);
				return -1;
			}


			/* Privilege separation */
			char user[256] = "";
			if (session_get_user (http_session, user, sizeof (user)) != 0)
			{
				odcgi_print_login_form (ODCGI_ERROR__UNKNOWN_USER, http_user);
				//odcgi_msg("Error: " ODCGI_ERROR__UNKNOWN_USER, "", 1);
				return -1;
			}

			syslog (LOG_NOTICE, "user: %s\n", user);
			syslog (LOG_NOTICE, "session: %s\n", http_session);
			if (session_store_session (user, http_session) != 0)
				syslog (LOG_WARNING, "session not saved: %s\n", http_session);

			sstrncpy (env->user, user, MAX_ENV_SIZE);
		}

		/* No log-in post neither valid session already started */
		else
		{
			//! @bug Issue 55 reaches this point.
			/* First access: no error */
			if ((strlen (http_pass) == 0) && (strlen (http_user) > 0))
			{			// User but no password specified
				odcgi_print_login_form ("", http_user);
			}
			else if ((strlen (http_pass) == 0) && (strlen (http_user) == 0))
			{			// No user NOR password specified
				odcgi_print_login_form ("", "");
			}
			else
			{
				syslog (LOG_WARNING,
						"ISSUE55, please notify it to OD developers\n");
				syslog (LOG_WARNING, "http_user: ->%s<-\n", http_user);
				syslog (LOG_WARNING, "http_pass: ->%s<-\n", http_pass);
				syslog (LOG_WARNING, "http_session: ->%s<-\n", http_session);

				odcgi_print_login_form (ODCGI_ERROR__SESSION, "");
			}
			return -1;
		}
	}
	return 0;
}

// }}}

// {{{ odcgi_logout()
/// Close the active session
int
odcgi_logout (script_env_t * env, const char *http_logout)
{
  if (strlen (http_logout) > 0 && http_logout[0] == '1')
    {
      logevent ("notice", "User [%s] logout", env->user);
      session_delete_session (env->user);
      odcgi_print_login_form ("", "");
      return 1;
    }
  return 0;
}

// }}}

// {{{ odcgi_check_root()
/// Return 1 if the user is root
int
odcgi_check_root (const char *http_user)
{
  if (strcmp ("root", http_user) == 0)
    {
      odcgi_print_login_form (ODCGI_ERROR__ROOT_NO_ACCESS, http_user);
      return 1;
    }
  return 0;
}

// }}}

// {{{ odcgi_add_user()
/// Wrapper function for user_add
int
odcgi_add_user (char *http_newuser, char *http_newpass1, char *http_newpass2)
{

  if (strlen (http_newuser) > 0)
    {
      if (strlen (http_newpass1) < 6)
	{
	  odcgi_msg_error (ODCGI_ERROR__USER_MGMT, "Password too short");
	  return -1;
	}

      if (strcmp (http_newpass1, http_newpass2) != 0)
	{
	  odcgi_msg_error (ODCGI_ERROR__USER_MGMT, "Incorrect password");
	  return -1;
	}

      user_t user;
      user_add (&user, http_newuser, http_newpass1);
      if (user_has_error (&user))
	{
	  odcgi_msg_error (ODCGI_ERROR__USER_MGMT,
		     user_get_error (&user));
	  return -1;
	}
      logevent ("notice", "User [%s] added", http_newuser);
    }

  return 0;
}

// }}}

// {{{ odcgi_del_user()
/// Wrapper function for user_del
int
odcgi_del_user (char *http_deluser)
{
  if (strlen (http_deluser) > 0)
    {
      user_t user;
      user_del (&user, http_deluser);
      if (user_has_error (&user))
	{
	  odcgi_msg_error (ODCGI_ERROR__USER_MGMT,
		     user_get_error (&user));
	  return -1;
	}
      logevent ("notice", "User [%s] deleted", http_deluser);
    }
  return 0;
}

// }}}

// {{{ odcgi_mod_user()
/// Change user password
int
odcgi_mod_user (char *http_moduser, char *http_modoldpass,
		char *http_modpass1, char *http_modpass2)
{
  if (strlen (http_moduser) > 0)
    {
      if (!session_is_valid_user (http_moduser, http_modoldpass))
	{
	  odcgi_msg_error (ODCGI_ERROR__USER_MGMT, "Incorrect password");
	  return -1;
	}

      if (strlen (http_modpass1) < 6)
	{
	  odcgi_msg_error (ODCGI_ERROR__USER_MGMT, "Password too short");
	  return -1;
	}

      if (strcmp (http_modpass1, http_modpass2) != 0)
	{
	  odcgi_msg_error (ODCGI_ERROR__USER_MGMT, "Incorrect new password");
	  return -1;
	}

      user_t user;
      user_set_password (&user, http_moduser, http_modpass1);
      if (user_has_error (&user))
	{
	  odcgi_msg_error (ODCGI_ERROR__USER_MGMT, user_get_error (&user));
	  return -1;
	}
      logevent ("notice", "User [%s] updated", http_moduser);
    }

  return 0;
}

// }}}



/* {{{ odcgi_print_file() */
/** Print plain text file 
 @param path File path
 @todo unify with http_send_file
 */
void
odcgi_print_file (const char *path)
{
  openlog ("odcgi", LOG_PID, LOG_USER);

  char filename[1024];
  // Building the full path
  snprintf (filename, sizeof (filename), "%s%s", OD_CFG_ROOT_DIR, path);
  file_t fs;
  file_set_filename (&fs, filename);
  syslog (LOG_NOTICE, "%s\n", filename);
  printf ("Cache-Control: max-age=3600, must-revalidate\n");
  printf ("Content-Type: text/plain\n\n");
  if (file_is_file (&fs))
    {
      printf ("/* %s: */\n", filename);
      file_print (&fs);
    }
  else
    {
      printf ("/* File (%s) does not exist */\n\n", filename);
    }
}

// }}} */

// {{{ getip()
/// Get the IP assigned to the specified interface
char *
getip (const char *iface)
{
  struct ifreq if_data;
  int sockd;
  u_int32_t ip;
  u_int32_t local_ip;
  struct in_addr in;

  if (getuid () != 0)
    {
      //perror ("You must be root. \n");
      return "";
    }

  if ((sockd = socket (AF_INET, SOCK_PACKET, htons (ETH_P_ARP))) < 0)
    {
      //perror("socket");
      return "";
    }

  strcpy (if_data.ifr_name, iface);

  if (ioctl (sockd, SIOCGIFADDR, &if_data) < 0)
    {
      //perror ("ioctl(); SIOCGIFADDR \n");
      return "";
    }

  memcpy (&ip, &if_data.ifr_addr.sa_data + 2, 4);
  //memcpy ((void *) &ip, (void *) &if_data.ifr_addr.sa_data + 2, 4);
  local_ip = ntohl (ip);
  in.s_addr = ip;

  return inet_ntoa (in);
}

// }}}


// {{{ main()
/** Main execution routine
 @param argc Number of args (0)
 @param argv Args (always empty)
 */
int
main (int argc, char *argv[])
{
	/* ---------------------------------------------------------------------
	*  Alert! setuid program with root privileges
	* ---------------------------------------------------------------------*/

	/* syslog */
	openlog ("odcgi", LOG_PID, LOG_USER);

	script_env_t env;


	/* Agent address */
	//! @todo: what if eth0 don't exists?
	snprintf (env.agent_address, MAX_ENV_SIZE, "%s", getip ("eth0"));

	//! @todo: lots of static variables. Maybe some can be reused to save memory
	char http_gui[8];
	//char http_style[10];
	char http_logout[8];
	char http_user[256];
	char http_pass[256];
	char http_session[1024];
	char http_noheader[8];
	char http_newuser[256];
	char http_newpass1[256];
	char http_newpass2[256];
	char http_deluser[256];
	char http_moduser[256];
	char http_modpass1[256];
	char http_modpass2[256];
	char http_modoldpass[256];
	char http_getfile[256];
	char http_resource[50];
	char http_play_mjpg[100];
	char http_temp[100];

	/* Configuration vars */
	FILE *fh;
	read_config_file (fh, OD_APP_I18N_CONF, lang, "en");
	//read_config_file(fh,OD_APP_STYLE_CONF,style,"default");
	//read_config_file(fh,OD_APP_SKIN_CONF,skin,"silver");

	/* Get HTTP variables */
	cgi_t *cgi = cgi_alloc ();

	cgi_get_param_by_name (cgi, "GUI", http_gui, sizeof (http_gui));
	cgi_get_param_by_name (cgi, "LOGOUT", http_logout, sizeof (http_logout));
	cgi_get_param_by_name (cgi, "USER", http_user, sizeof (http_user));
	for (int c=0;c<sizeof(http_user);c++){
		if (http_user[c]=='@') http_user[c]=0; // Cutting e-mail to uid
	}  
	cgi_get_param_by_name (cgi, "PASS", http_pass, sizeof (http_pass));
	cgi_get_param_by_name (cgi, "HTSESSID", http_session,
	 sizeof (http_session));
	cgi_get_param_by_name (cgi, "NOHEADER", http_noheader,
	 sizeof (http_noheader));
	cgi_get_param_by_name (cgi, "NEWUSER", http_newuser, sizeof (http_newuser));
	cgi_get_param_by_name (cgi, "NEWPASS1", http_newpass1,
	 sizeof (http_newpass1));
	cgi_get_param_by_name (cgi, "NEWPASS2", http_newpass2,
	 sizeof (http_newpass2));
	cgi_get_param_by_name (cgi, "DELUSER", http_deluser, sizeof (http_deluser));
	cgi_get_param_by_name (cgi, "MODUSER", http_moduser, sizeof (http_moduser));
	cgi_get_param_by_name (cgi, "MODPASS1", http_modpass1,
	 sizeof (http_modpass1));
	cgi_get_param_by_name (cgi, "MODPASS2", http_modpass2,
	 sizeof (http_modpass2));
	cgi_get_param_by_name (cgi, "MODOLDPASS", http_modoldpass,
	 sizeof (http_modoldpass));
	cgi_get_param_by_name (cgi, "FILE", http_getfile, sizeof (http_getfile));
	cgi_get_param_by_name (cgi, "resource", http_resource,
	 sizeof (http_resource));
	cgi_get_param_by_name (cgi, "play_mjpg", http_play_mjpg,
	 sizeof (http_play_mjpg));



//      if (cgi_get_param_by_name(cgi,"style", http_style, sizeof(http_style))==1)
//      {
//              //cgi_get_cookie("HTSTYLE", http_style, sizeof(http_style));
//              strncpy(style, http_style, sizeof(http_style));
//              cgi_http_header_set_cookie("HTSTYLE", style);
//      }

/* DEPRECATED
	// Si se ha solicitado una hoja de estilo, la entregamos
	if (cgi_get_param_by_name (cgi, "css", http_temp, sizeof (http_temp)) == 1)
    {
      syslog (LOG_NOTICE, "printing style: %s\n", http_temp);
      odcgi_print_file (http_temp);
      cgi_free (cgi);
      return 0;
    }
*/	
/*	DEPRECATED
	// Si se ha solicitado el javascript específico, lo entregamos
	if (cgi_get_param_by_name(cgi, "js", http_temp, sizeof(http_temp))==1) 
	{
		syslog(LOG_NOTICE, "printing script: %s\n", http_temp);
		odcgi_print_file(http_temp);
		return 0;
	} */


  if (strlen (http_session) == 0)
    {
      cgi_get_cookie ("HTSESSID", http_session, sizeof (http_session));
      syslog (LOG_NOTICE, "session from cookie: %s\n", http_session);
    }

  /* get gui type */
  if (strcmp (http_gui, "XML") == 0)
    gui = xml;
  if (strcmp (http_gui, "none") == 0)
    gui = none;


  /* login process */
  if (odcgi_login (&env, http_user, http_pass,
		   http_session, sizeof (http_session)) == -1)
    {
      syslog (LOG_NOTICE, "login failed: %s-%s\n", http_user, http_pass);
      cgi_free (cgi);
      return -1;
    }

//      syslog(LOG_NOTICE, "env.user: %s\n", env.user);
//      syslog(LOG_NOTICE, "http_user: %s\n", http_user);


  /* check logout */
  if (odcgi_logout (&env, http_logout))
    {
      cgi_free (cgi);
      return -1;
    }


  /* ---------------------------------------------------------------------
   *  Login OK: odcgi is setuid root
   * --------------------------------------------------------------------- */
  //syslog(LOG_NOTICE, "[odcgi] userid: %d\n", getuid());

  /* root has not access */
  if (odcgi_check_root (http_user))
    {
      cgi_free (cgi);
      return -1;
    }


  /* ---------------------------------------------------------------------
   *  Login OK:
   *  + admin has root privileges
   *  + normal user has Linux privileges
   * --------------------------------------------------------------------- */
	syslog (LOG_NOTICE, "[odcgi] user: %s, uid: %d, guid: %d\n",
		env.user, getuid (), getgid ());

	/* NO USER MANAGEMENT FUNCTIONS IN 2.2
  // adds a new user 
  if (odcgi_add_user (http_newuser, http_newpass1, http_newpass2) == -1)
    {
      cgi_free (cgi);
      return -1;
    }

  // delete user 
  if (odcgi_del_user (http_deluser) == -1)
    {
      cgi_free (cgi);
      return -1;
    }

  // modify user password 
  if (odcgi_mod_user (http_moduser, http_modoldpass,
		      http_modpass1, http_modpass2) == -1)
    {
      cgi_free (cgi);
      return -1;
    }

	*/

  /* set session */
  /* Privilege separation: drop root privileges */
//   syslog(LOG_NOTICE, "set session %s\n", http_session);
//   syslog(LOG_NOTICE, "[odcgi] session_set_ids user: %s\n", env.user);
	session_set_ids (env.user);
	syslog (LOG_NOTICE, "[odcgi] dropped privileges user: %s, uid: %d, guid: %d\n",
		env.user, getuid (), getgid ());
  

  /* File reference with user permissions applied */
	if (strlen (http_getfile) > 5)
    {
		char buffer[1024] = "/media/";
		strcat (buffer, http_getfile);
		if (http_send_file (buffer))
		{
			cgi_free (cgi);
			return 0;
		}
		else
		{
		//! @todo Mostrar error
		}
    }


  /* play mjpg file 
  if (strlen (http_play_mjpg) > 3)
	{
		syslog (LOG_NOTICE, "play: %s\n", http_play_mjpg);
		mjpg_play (http_play_mjpg);
		cgi_free (cgi);
		return 0;
	}

	switch (gui)
	{
		case xml:
			cgi_http_header_begin ("text/xml");
		break;
		case html:
			cgi_http_header_begin ("text/html");
		break;
		default:
			cgi_http_header_begin ("text/plain");
	}
 */

  /* Resource reference
  //TODO Verificar permisos de usuario
  if (strlen (http_resource) > 3)
	{
		syslog (LOG_NOTICE, "Serving resource %s\n", http_resource);
		if (mjpg_streaming_rsc (http_resource))
		{
			cgi_free (cgi);
			return 0;
		}
		else
		{
			//printf("<div id='connfail'><p class='error'>%s</p></div>\n",
			//              T(ODCGI_ERROR__CONNECTION_FAILURE));
			cgi_free (cgi);
			return 0;
		}
	} */

	syslog (LOG_NOTICE, "1.session: %s\n", http_session);
	cgi_http_header_set_cookie ("HTSESSID", http_session);
	//   cgi_get_cookie("HTSTYLE", style, sizeof(style));
	//   cgi_http_header_set_cookie("HTSTYLE", style);
	cgi_http_header_end ();




	/* ---------------------------------------------------------------------
	*  User privileges
	* ---------------------------------------------------------------------*/

	size_t len = cgi->decoded_url->size;
	char path_info[256 + len];
	path_info[0] = 0;
	sstrncpy (path_info, cgi_get_path_info (cgi), sizeof (path_info));
	syslog (LOG_NOTICE, "path_info %s - %d\n", path_info, strlen (path_info));



	// If any POST/GET vars matches with submit_X.sh, X.sh is the target
	// then replace 'path_info'.
	char options[256 + len];
	int index = 0;
	char varname[256];
	char varvalue[256 + len];
	int has_option = 0;
	char scriptname[256] = "";
	char path[256 + len];
	path[0] = 0;

	while (cgi_get_param_by_index (cgi, index, varname, sizeof (varname),
				 varvalue, sizeof (varvalue)))
	{
		// Replace "+" for " "
		for (int i=0;i<sizeof(varvalue);i++) {
			if (varvalue[i]=='+') varvalue[i]=' ';
		}

		syslog (LOG_DEBUG, "CGIParam %d %s=%s\n", index, varname, varvalue);
		
		if (strcmp (varname, "GUI") == 0)
		{
		// Ignore               

		}
		else if (strcmp (varname, "odcgioptionsel") == 0)
		{
			sstrncat (options, varvalue, sizeof (options));
			sstrncat (options, " ", sizeof (options));
			has_option = 1;
		}

		else if (strncmp (varname, "submit_", 7) == 0)
		{
			syslog (LOG_NOTICE, "Submit redirection found at %s\n", varname);
			sstrncpy (scriptname, varname + 7, sizeof (scriptname));
			//sstrncpy(path_info, scriptname, sizeof(path_info));
			snprintf (path, sizeof (path), "/usr/local/opendomo/%s",
			    scriptname);
			syslog (LOG_DEBUG, "debugging %s - %s [%s]\n", scriptname, path,
			  options);
			break;
		}
		index++;
	}


	/* Check PATH variable */
	if (strlen (path_info) == 0)
		strcpy (path_info, "/");

	/* filters */
	if (!match(path_info,
		"^/[a-záéíóúàèäëïöüñçA-ZÁÉÍÓÚÀÈÄËÏÖÜÑÇ0-9_/]*\\.{0,1}[a-záéíóúàèäëïöüñçA-ZÁÉÍÓÚÀÈÄËÏÖÜÑÇ0-9_/+ =?:]*$"))
    {
		odcgi_print_header ("error", env.user);
		syslog (LOG_ERR, "%s\n", ODCGI_ERROR__INVALID_PATH);
		odcgi_msg_error (ODCGI_ERROR__INVALID_PATH,
		"Invalid character found in the command.");
		printf ("\n<!-- PATH_INFO: %s-->\n", path_info);
		odcgi_print_footer ("", 0, cgi);
		cgi_free (cgi);
		return -1;
    }

	int err = 0;

	char *param_regex = "[$;'\\\"]";
	if (strstr (cgi_get_query_string (cgi), ".."))
		err = 1;
	else if (strstr (cgi_get_decoded_url (cgi), ".."))
		err = 2;
	else if (strlen (cgi_get_query_string (cgi)) > 0 &&
		match (cgi_get_query_string (cgi), param_regex))
		err = 3;
	else if (strlen (cgi_get_decoded_url (cgi)) > 0 &&
		match (cgi_get_decoded_url (cgi), param_regex))
		err = 4;

	if (err!=0)
    {
		odcgi_print_header ("error", env.user);
		syslog (LOG_ERR, "%s\n", ODCGI_ERROR__INVALID_PATH);
		odcgi_msg_error (ODCGI_ERROR__INVALID_PATH,
			"Invalid character found in the parameters.");
		printf ("\n<!-- PATH ERROR: %d (not allowed) \n\t%s \n\t %s -->\n", 
			err,
			cgi_get_query_string (cgi),
			cgi_get_decoded_url (cgi));
		odcgi_print_footer ("", 0, cgi);
		cgi_free (cgi);
		return -1;
    }

	// If PATH is not modified, use the default path in CONF_DIR.
	if (path[0] == 0)
    {
		snprintf (path, sizeof (path), "%s/%s", OD_CFG_ROOT_DIR, path_info);
    }

  /* root directory */
  if (chdir (OD_CFG_ROOT_DIR) != 0)
    {
      odcgi_print_header ("error", env.user);
      syslog (LOG_ERR, "%s\n", ODCGI_ERROR__ROOT_PATH_ACCESS);
      odcgi_msg_error (ODCGI_ERROR__ROOT_PATH_ACCESS,
		 "Cannot access the configuration directory. "
		 "Missing privileges or misconfiguration");
      odcgi_print_footer ("", 0, cgi);
      cgi_free (cgi);
      return -1;
    }


	char name[256 + len];
	char value[256 + len];
	char prename[256 + len];
	char *shname;
	string_t *cmd = string_alloc ("");

	file_t fs;
	strcpy (scriptname, basename (path));

	/* HTML-head begin */
	odcgi_print_header (scriptname, env.user);

	printf ("<!-- path: %s, path_info: %s-->\n", path, path_info);

	/* Check NOHEADER */
	if ((gui == html) && (atoi(http_noheader) != 1))
    {
		printf("<div id='header'>");
		printf("	<ul id='categ' class='categories'>");
		printf("		<li id='cat-control' class='tab'><a href='/cgi-bin/od.cgi/control'>%s</a></li>",T("Control"));
		printf("		<li id='cat-tools'  class='tab'><a href='/cgi-bin/od.cgi/tools'>%s</a></li>",T("Applications"));
		printf("		<li id='cat-config' class='tab'><a href='/cgi-bin/od.cgi/config'>%s</a></li>",T("Configuration"));
		if (1000 == getgid()) { // Only "admin" can save system configuration
			printf ("\t\t<li id='cat-save' class='tab'><a href='/cgi-bin/od.cgi/saveConfig.sh' >%s</a></li>\n", T(ODCGI_TEXT_SAVE));			
		}	
		printf("		<li id='cat-logout' class='tab'><a href='?LOGOUT=1'>%s</a></li>\n", T (ODCGI_TEXT_LOGOUT));		
		printf("	</ul>");
		printf("</div>");
	  
		//string_assign_str (cmd, "/usr/bin/categories.sh ");
		//string_append (cmd, path_info);

		//script_exec (cmd->str, "header", &env);
		//if (strlen (path_info) < 2)
		//{
		//  printf
		//    ("  <div class='applicationTitle'><h1>OpenDomo</h1></div>\n");
		//}    else	{
		//  printf ("  <div class='root'><a href='" OD_URI "/'> </a></div>\n");
		//}
    }



	sstrncpy (prename, path, sizeof (prename));
	shname = strtok (prename, " ");
	file_set_filename (&fs, shname);
	strcpy (scriptname, basename (path));

	/* if dir: list contents */
	if (file_is_dir (&fs))
    {
      string_assign_str (cmd,  "/usr/bin/list.sh ");
      string_append (cmd, path_info);
      string_append (cmd, " contents");

      script_exec (cmd->str, "main", &env);
    }
	else
    {
		  /* if file: execute */
		  // The path might be a redirection (no actual link in ..opendomo/root/)
		if (!file_is_file (&fs))
		{
		  snprintf (path, sizeof (path), "/usr/local/opendomo/%s",
				basename (scriptname));
		  printf ("\n<!-- debug paths: %s / %s [%s] -->\n", path, path_info,
			  scriptname);
		  file_set_filename (&fs, path);
		  if (!file_is_file (&fs))	// If it's still not a valid path, abort
			{
			  odcgi_msg_error (ODCGI_ERROR__SCRIPT_NOT_FOUND,
				 "The script was not found. "
				 "Maybe the function you are requiring "
				 "is not installed in this system");
			  printf ("<!-- BASENAME: %s -->\n", basename (scriptname));
			  odcgi_print_footer ("", 0, cgi);
			  cgi_free (cgi);
			  return 1;
			}
		}
		//printf("<!-- debug path: %s -->\n", path);
		//char *p = strrchr(path, '/');
		if (has_option /*&& p */ )
		{
			string_assign_str (cmd, path);
			string_append (cmd, " ");
			string_append (cmd, options);
		}
		else
		{
			string_assign_str (cmd, path);
			string_append (cmd, " ");
		}
		printf ("\n<!-- decoded_url: %s \n\tquery_string: %s-->\n", 
			cgi->decoded_url->str, cgi->query_string->str);
		int i = 0;
		while (cgi_get_param_by_index (cgi, i++,
			name, sizeof (name), value, sizeof (value)))
		{
			if (strcmp (name, ODCGI_SESSION_NAME) == 0)
			{
				// Ignoring session name var ...
			}
			else if (strncmp (name, "GUI", 3) == 0)
			{
				// Ignoring GUI param
			}
			else if (strncmp (name, "submit_", 7) == 0)
			{
				// Ignoring possible submit redirection ...
			}
			else
			{
				//for (i = 0; i < sizeof(value); i++){
				//	if (value[i]=='+') value[i]=' ';
				//}
				// Avoid overwritting a defined environment var
				if (getenv (name) == NULL)
					setenv (name, value, 1);
				string_append (cmd, " \"");
				string_append (cmd, value);
				string_append (cmd, "\" ");
			}
		}
		string_replace (cmd, "+", " ");
		string_replace (cmd, "'", "&apos;");

		printf ("<!-- cmd (file): %s -->\n", cmd->str);
		//fflush(stdout); // Force flush, otherwise an error will preceed stdout
		// Check the returned value of script_exec()


		int ret = script_exec (cmd->str, "main", &env);
		if (ret != 0)
		{
			/* else: empty div */
			printf ("<div id='main'><p class='error'>%s</p></div>",
				ODCGI_ERROR__SCRIPT_NOT_FOUND);
		}
    }
	/* Print scripts */
	//odcgi_print_script(path); DEPRECATED

	/* HTML end */
	if (atoi (http_noheader) != 1)
    {
      odcgi_print_footer ("", BUTTON_LOGOUT + BUTTON_DEBUG, cgi);
    }

	string_free (cmd);
	cgi_free (cgi);
	closelog ();
	return 0;
}
// }}}
