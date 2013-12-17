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
  @file i18n.c
  @brief Translation functions
 */

#include <syslog.h>
#include "conf.h"
#include "i18n.h"

#include <src/str.h>

#define TRUE 1		///< True int val
#define FALSE -1	///< False int val

extern gui; ///< Expected output

/// Get the translated string by its ID
int get_string_by_id(int id, const char *lang, char *tr, size_t trlen)
{
   char *p = NULL;
   char buffer[1024];
	tr[0]=0;

   //syslog(LOG_NOTICE, "[i18n] translate id:%d, lang:%s\n", id, lang);

   char file[256];
   snprintf(file, sizeof(file), OD_APP_I18N_DIR "/%s", lang);

   FILE *f = fopen(file, "r"); 
   if(!f)
   {
      syslog(LOG_NOTICE, "[i18n] translate error. Cannot open file %s\n", file);
      tr[0]=0;
      return FALSE;
   }

   char line[1024];
   while(!feof(f))
   {
      /* read line */
      fgets(line, sizeof(line), f);


      p = str_split(line, ":", buffer, sizeof(buffer));
      str_trim(buffer);
      int new_id = atoi(buffer);

      p = str_split(p, ":", buffer, sizeof(buffer));
      str_trim(buffer);

      /* id */
      if(id == new_id)
      {
         snprintf(tr, trlen, "%s", buffer);
         //syslog(LOG_DEBUG, "[i18n] translation ok: -%s-\n", tr);
         return TRUE;
      }
   }

   syslog(LOG_NOTICE, "[i18n] id not found: %d\n", id);
   fclose(f);
   return FALSE;
}

/// Removes the variable part of the string and stores it in var
int trans(const char *text, char*buffer, size_t blen, char*var, size_t vlen)
{
	char *pcs = NULL;
	char *pce = NULL;
	int count=0;
	var[0]=0;

	if(!strstr(text, "["))
	{
		snprintf(buffer, blen, "%s", text);
		return 0;
	}

	pcs = strtok ((char*)text,"[");
	pce = strtok (NULL, "]");

	if (pcs!=NULL && pce!=NULL)
	{
		strncpy(var, pce, vlen);
		pce = strtok (NULL, "]");

		if(pce!=NULL)
			snprintf(buffer, blen, "%s%%s%s", pcs, pce);
		else
			snprintf(buffer, blen, "%s%%s", pcs);
		count++;
	}

	return count;
}

char* get_translated_string(const char* text, const char* lang, int verbose)
{

	   char *p = NULL;
	   char var[1024]="\0";
	   char buff[1024];
	   char buff_novar[1024];

	   static char text_input[1024];
	   static char buffer[1024];

	   if(0==trans(text, text_input, sizeof(text_input), var, sizeof(var)))
	   {
	      snprintf(text_input, sizeof(text_input), "%s", text);
	   }
	   str_trim(text_input);

	   snprintf(buffer, sizeof(buffer), "%s", text_input);

	   if(strcmp(lang, "default")==0)
	   {
	      return buffer;
	   }

	   FILE *f = fopen(OD_APP_I18N_DIR "/key", "r");
	   if(!f)
	   {
			if (gui==1 && verbose==TRUE) // Only if HTML output
			{
				snprintf(buffer, sizeof(buffer),
				"<i class='untrans' lang='%s'>%s</i>", lang, text);
				return buffer;
			}
			else
			{
				return (char*)text;
			}
	   }

	   char line[1024];
	   while(!feof(f))
	   {
	      /* read line */
	      fgets(line, sizeof(line), f);

	      /* split id and string, ex: 23: Hello World */
	      p = str_split(line, ":", buff, sizeof(buff));
	      str_trim(buff);
	      int id = atoi(buff);

	      p = str_split(p, ":", buff, sizeof(buff));
	      str_trim(buff);

	      //syslog(LOG_DEBUG, "[i18n] cmp -%s- -%s-\n", text_input, buff);
	      snprintf(buff_novar, sizeof(buff_novar), "%s", text_input);
		  str_replace(buff_novar, sizeof(buff_novar), "%s", "");
		  str_trim(buff_novar);
		  if(strcmp(buff, text_input)==0 || strcmp(buff, buff_novar)==0)
		  {
				//syslog(LOG_DEBUG, "[i18n] cmp -%s- = -%s-\n", text_input, buff);
				/* translated string */
				char translated[1024];
				if (get_string_by_id(id, lang, translated, sizeof(translated))==FALSE)
				{
					//strncpy(buffer, translated, sizeof(buffer));
					fclose(f);
					if (gui==1 && verbose==TRUE) // Only if HTML output
					{
						snprintf(buffer, sizeof(buffer),
						"<i class='untrans' id='%d' lang='%s'>%s</i>", id, lang, text);
						return buffer;
					}
					else
					{
						return (char*)text;
					}
				}
				else {
					fclose(f);
					//syslog(LOG_DEBUG, "[i18n] is translated!! -%s- \n", translated);
					if (var[0]!=0)
						snprintf(buffer, sizeof(buffer), translated, var);
					else
						strncpy(buffer, translated, sizeof(buffer));
					return buffer;
				}
			}
		}

		fclose(f);
		syslog(LOG_DEBUG, "[i18n] string not found! '%s' \n", text_input);
		if (var[0]!=0)
			snprintf(buffer, sizeof(buffer), text_input,var);
		else
		{
			if (gui==1 && verbose==TRUE) // Only if HTML output
			{
				snprintf(buffer, sizeof(buffer),
				"<i class='untrans' lang='%s'>%s</i>", lang, text_input);
			}
			else
			{
				return text_input;
			}
		}

		//syslog(LOG_DEBUG, "[i18n] return -%s-\n", buffer);
		return buffer;

}

/// Translates the text to the specified language
char* i18n(const char* text, const char* lang, int verbose)
{
  char * string_res;
  char var[1024]="\0";
  static char text_input[1024];
  static char buffer[1024];


  snprintf(buffer, sizeof(buffer), "%s", text);
  trans(buffer, text_input, sizeof(text_input), var, sizeof(var));

  //remove empty vars
  str_replace(buffer, sizeof(buffer), "[ ]", "");

  if (var[0]!=0 && var[0]!=' ')
  {
	  char * string_var;
	  static char buffer_trans_var[1024];

	  // getting translated variable
	  string_var = get_translated_string(var, lang, verbose);
	  snprintf(buffer_trans_var, sizeof(buffer_trans_var), string_var);

	  //replace it
	  string_res = get_translated_string(text, lang, verbose);
	  snprintf(buffer, sizeof(buffer), string_res);
 	  str_replace(buffer, sizeof(buffer), var, buffer_trans_var);

      syslog(LOG_NOTICE, "[i18n] %s = %s = %s\n", string_var, string_res, buffer_trans_var);
 	  return buffer;
  }
  else
	  string_res = get_translated_string(text, lang, verbose);

  return string_res;
}
