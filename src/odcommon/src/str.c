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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include <sys/time.h>
#include <src/str.h>
#include <src/vector.h>

// {{{ str_assign_str()
void str_assign_str(char *o, size_t olen, const char *s)
{
   if(!o)
      return;

   snprintf(o, olen, "%s", s);
}
// }}}

// {{{ str_assign_int()
void str_assign_int(char *o, size_t olen, int value)
{
   if(!o)
      return;

   snprintf(o, olen, "%d", value);

} // }}}

// {{{ str_assign_float()
void str_assign_float(char *o, size_t olen, float value)
{
   if(!o)
      return;

   snprintf(o, olen, "%6f", value);
} // }}}

// {{{ str_to_int()
int str_to_int(char *o)
{
   if(!o)
      return 0;

   return atoi(o);
}
// }}}

// {{{ str_to_float()
float str_to_float(char *o)
{
   if(!o)
      return 0;

   float f;
   sscanf(o, "%f", &f);
   return f;
}
// }}}

// {{{ str_trim()
void str_trim(char *str)
{
   if(!str)
      return;

   char *ptr1 = str;
   char *ptr2 = str;

   while(isspace(*ptr2)) 
      ptr2++; 

   while(*ptr2!=0) 
      *ptr1++ = *ptr2++;
         
   ptr1--;
   while(isspace(*ptr1)) 
      ptr1--; 

   *(ptr1+1) = 0;
}
// }}}

// {{{ str_split()
char* str_split(char *str, const char *token, char *res, size_t reslen)
{
   if(!str)
      return str;

   if(!reslen)
      return str;

   snprintf(res, reslen, "%s", str);

   /* find token */
   char *p = strstr(res, token);
   if(!p)
   {
      return NULL;
   }

   /* get result */
   *p = 0;

   return str+strlen(res)+strlen(token);
}
// }}}

// {{{ str_replace()
void str_replace(char *str, size_t slen, 
                 const char *pattern, const char *replacement)
{ 
   int n_src=0; 
   char *ptr = str; 
   char *ptr2 = str; 
   
   while((ptr=strstr(ptr, pattern))) 
   { 
      n_src++; 
      ptr++; 
   }
   int new_size = strlen(str) + 
      n_src*(strlen(replacement)-strlen(pattern)); 

   char string[new_size+1]; 
   string[0]=0; 
   ptr = str; 

   while((ptr=strstr(ptr, pattern))) 
   { 
      int len = ptr-ptr2; 
      strncat(string, ptr2, len); 
      strncat(string, replacement, sizeof(string)); 
      string[sizeof(string)-1]=0; 
      ptr2 = ptr+strlen(pattern); 
      ptr  = ptr+strlen(pattern); 
   } 

   int len = ptr-ptr2; 
   strncat(string, ptr2, len); 
   
   snprintf(str, slen, "%s", string);
}
// }}}

// {{{ str_regex_match()
int str_regex_match(const char *str, const char *regex)
{
  int r;
   regex_t expr;
   regmatch_t rm;

   if( (r=regcomp(&expr, regex, REG_EXTENDED))!= 0 ) 
      return 0;
   
   if( (r=regexec(&expr, str, 0, &rm, 0))!=0 )
   {
      regfree(&expr);
      return 0;
   }

   regfree(&expr);
   return 1;
}
// }}}

// {{{ str_append()
void str_append(char *o, size_t olen, const char *str)
{
   if(!o)
      return;

   strncat(o, str, olen);
   o[olen-1]=0;
}
// }}}

// {{{ str_cut()
void str_cut(char *o, size_t olen, size_t begin, size_t end)
{
   if(!o)
      return;

   int e = end;
   if(e>strlen(o))
      e = strlen(o); 

   if(begin<=e) 
   {
      char *ptr = o+begin;
      strncpy(o, ptr, e-begin);
      o[e-begin]=0; 
   } 

}
// }}}

// {{{ str_find()
size_t str_find(char *o, const char *pattern)
{
   if(!o)
      return 0;

   char *ptr = strstr(o, pattern);
   if(!ptr)
      return -1;

   return ptr - o;
}
// }}}

// {{{ str_lpad()
void str_lpad(char *o, size_t olen, char padd_char, size_t total_len)
{
   if(!o)
      return;

   if(strlen(o)<total_len) 
   {
      int i;
      int tlen = total_len;
      
      if(tlen>olen)
         tlen=olen-1;
      
      char string[tlen+1]; 

      strncpy(string, o, sizeof(string)); 
      string[sizeof(string)-1]=0;

      for(i=0; i<tlen-strlen(string); i++) 
         o[i]=padd_char;

      o[i]=0;
      strncat(o, string, tlen);
      o[tlen]=0;
   }
}
// }}}

// {{{ str_rpad()
void str_rpad(char *o, size_t olen, char padd_char, size_t total_len)
{
   if(strlen(o)<total_len)
   { 
      int i;
      int len = strlen(o);
      int tlen = total_len;
      
      if(tlen>olen)
         tlen=olen-1;

      for(i=len; i<tlen; i++)
         o[i]=padd_char;
      o[tlen]=0;
   }
}
// }}}

// {{{ str_to_lower()
void str_to_lower(char *o, size_t olen)
{
	int i;
	for(i=0; i<olen; i++)
		o[i]=tolower(o[i]);
}
// }}}

// {{{ str_to_upper()
void str_to_upper(char *o, size_t olen)
{
	int i;
	for(i=0; i<olen; i++)
		o[i]=toupper(o[i]);
}
// }}}

// {{{ str_random()
int str_random(char *o, size_t olen, int rndlen, const char *validchars)
{
	if(rndlen > olen-1)
		return -1;

	int i;
	int l=strlen(validchars);
	int seed = 0;

   FILE *random_file = fopen("/dev/urandom","r");
   if(!random_file)
		return -2;
      
   struct timeval tv;
   gettimeofday(&tv, NULL);


	seed = getc(random_file)+tv.tv_usec;
   srandom(seed);

   for(i=0; i<rndlen; i++)
   {
      int idx = random()%l;
      o[i]=validchars[idx];
   }

   o[i]=0;
   fclose(random_file);
	return 0;
}
// }}}
