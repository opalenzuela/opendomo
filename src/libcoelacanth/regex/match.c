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
#include <regex.h>
#include "match.h"

// {{{ match_ext()
/*
 * allocate "len" results in result[] with malloc().
 *
 * results[0] -> all match
 * results[1] -> first match
 * results[2] -> second match
 * ...
 */
int match_ext(const char *string, const char *pattern, 
      char *result[], size_t len)
{
   int r;
   regex_t expr;
   regmatch_t rm[len];
   result[0]=0;
   int pos=0;

   if( (r=regcomp(&expr, pattern, REG_EXTENDED))!= 0 ) 
      return 0;
   
   if( (r=regexec(&expr, string, len, rm, 0))!=0 )
   {
      regfree(&expr);
      return 0;
   }

   for(pos=0; pos<len; pos++)
   {
      if(rm[pos].rm_so<0)
      {
         result[pos] = malloc(1);
         result[pos][0]=0;
         continue;
      }

      if(rm[pos].rm_so>=0)
      {
         result[pos] = malloc(rm[pos].rm_eo-rm[pos].rm_so+1);
         strncpy(result[pos], &string[rm[pos].rm_so], 
               rm[pos].rm_eo-rm[pos].rm_so+1);
         result[pos][rm[pos].rm_eo-rm[pos].rm_so]=0;
      }
   }

   regfree(&expr);
   return 1;
}
// }}}

// {{{ match_ext2()
int match_ext2(const char *string, const char *pattern, int pos,  
      char *result, size_t len)
{
   int r;
   regex_t expr;
   regmatch_t rm[100];
   result[0]=0;

   if( (r=regcomp(&expr, pattern, REG_EXTENDED))!= 0 ) 
      return 0;
   
   if( (r=regexec(&expr, string, 100, rm, 0))!=0 )
   {
      regfree(&expr);
      return 0;
   }

   if(pos<100 && rm[pos].rm_so>=0 && rm[pos].rm_eo-rm[pos].rm_so<len)
   {
      strncpy(result, &string[rm[pos].rm_so], len);
      result[rm[pos].rm_eo-rm[pos].rm_so]=0;
   }

   regfree(&expr);
   return 1;
}
// }}}

// {{{ match()
int match(const char *string, const char *pattern)
{
   int r;
   regex_t expr;
   regmatch_t rm;

   if( (r=regcomp(&expr, pattern, REG_EXTENDED))!= 0 ) 
      return 0;
   
   if( (r=regexec(&expr, string, 0, &rm, 0))!=0 )
   {
      regfree(&expr);
      return 0;
   }

   regfree(&expr);
   return 1;
}
// }}}



