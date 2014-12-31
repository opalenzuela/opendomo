 /*****************************************************************************
 *  This file is part of the OpenDomo project.
 *  Copyright(C) 2015 OpenDomo Services SL
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


#define _GNU_SOURCE
#include <src/string.h>

/* ---------------------------------------------------------------------------
 *   String Data Type
 * ------------------------------------------------------------------------ */

// {{{ string_alloc()
string_t *string_alloc(const char *s)
{
   string_t *o = malloc(sizeof(string_t));
   o->size = 0;
   
   if(s)
	{
      o->str = strdup(s);
		o->size = strlen(s);
	}
   else
      o->str = strdup("");

   return o;
}
// }}}

// {{{ string_free()
void string_free(string_t *o)
{
   if(o) 
      free(o);

   o = NULL;
}
// }}}

// {{{ string_assign_str()
void string_assign_str(string_t *o, const char *s)
{
   if(!o)
      return;

   if(o->str)
      free(o->str);

   o->str = strdup(s);
   o->size = strlen(o->str);
}
// }}}

// {{{ string_assign_int()
void string_assign_int(string_t *o, int value)
{
   if(!o)
      return;

   if(o->str)
      free(o->str);

   int intvalue = value;
   int i=0;

   while(value>0) 
   { 
      i++; 
      value /= 10; 
   } 

   char str[i+2];
   snprintf(str, sizeof(str), "%d", intvalue); 

   o->str = strdup(str);
   o->size = strlen(o->str);

} // }}}

// {{{ string_assign_float()
void string_assign_float(string_t *o, float value)
{
   if(!o)
      return;

   if(o->str)
      free(o->str);

   int i=0;
   float floatvalue = value;

   while(value>0) 
   { 
      i++; 
      value /= 10; 
   } 
  
   char str[i+9];
   snprintf(str, sizeof(str), "%6f", floatvalue); 

   o->str = strdup(str);
   o->size = strlen(o->str);

} // }}}

// {{{ string_to_int()
int string_to_int(string_t *o)
{
   if(!o)
      return 0;
   
   return atoi(o->str);
}
// }}}

// {{{ string_to_float()
float string_to_float(string_t *o)
{
   if(!o)
      return 0;
   
   float f;
   sscanf(o->str, "%f", &f);
   return f;
}
// }}}

// {{{ string_trim()
void string_trim(string_t *o)
{
   if(!o)
      return;

   if(!o->str)
      return;

   char *ptr1 = o->str;
   char *ptr2 = o->str;

   while(isspace(*ptr2)) 
      ptr2++; 

   while(*ptr2!=0) 
      *ptr1++ = *ptr2++;
         
   (*ptr1)--;
   while(isspace(*ptr1)) 
      ptr1--; 

   *(ptr1+1) = 0;
}
// }}}

// {{{ string_replace()
void string_replace(string_t *o, const char *pattern, const char *replacement)
{
   if(!o)
      return;
 
   int n_src=0; 
   char *ptr = o->str; 
   char *ptr2 = o->str; 
   
   while((ptr=strstr(ptr, pattern))) 
   { 
      n_src++; 
      ptr++; 
   }
   int new_size = strlen(o->str) + 
      n_src*(strlen(replacement)-strlen(pattern)); 

   char string[new_size+1]; 
   string[0]=0; 
   ptr = o->str; 

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
   
   o->str = strdup(string);
   o->size = strlen(string);
}
// }}}

// {{{ string_append()
void string_append(string_t *o, const char *str)
{
   if(!o)
      return;

   char string[strlen(str)+strlen(o->str)+1];
   snprintf(string, sizeof(string), "%s%s", o->str, str);

	string_free(o);
	o = string_alloc(string);
}
// }}}

// {{{ string_cut()
void string_cut(string_t *o, size_t begin, size_t end)
{
   if(!o)
      return;

   int e = end;
   if(e>strlen(o->str))
      e = strlen(o->str); 

   if(begin<=e) 
   { 
      char *ptr = o->str+begin;
      strncpy(o->str, ptr, e-begin); 
      o->str[e-begin]=0; 
      o->size = strlen(o->str);
   } 
   
}
// }}}

// {{{ string_find()
size_t string_find(string_t *o, const char *pattern)
{
   if(!o)
      return 0;

   char *ptr = strstr(o->str, pattern);
   if(!ptr)
      return -1;

   return ptr - o->str;
}
// }}}

// {{{ string_lpad()
void string_lpad(string_t *o, char padd_char, size_t total_len)
{
   if(!o)
      return;

   if(strlen(o->str)<total_len) 
   { 
      int i; 
      int tlen = total_len; 
      char string[tlen+1]; 

      strncpy(string, o->str, sizeof(string)); 
      string[sizeof(string)-1]=0; 

      o->str = realloc(o->str, tlen+1); 
      for(i=0; i<tlen-strlen(string); i++) 
         o->str[i]=padd_char; 

      o->str[i]=0; 
      strncat(o->str, string, tlen); 
      o->str[total_len]=0; 
      o->size = strlen(o->str);
   }
}
// }}}

// {{{ string_rpad()
void string_rpad(string_t *o, char padd_char, size_t total_len)
{
   if(strlen(o->str)<total_len) 
   { 
      int i; 
      int len = strlen(o->str); 
      int tlen = total_len; 
      o->str = realloc(o->str, tlen+1); 

      for(i=len; i<tlen; i++) 
         o->str[i]=padd_char; 
      o->str[tlen]=0; 
   }
}
// }}}

// {{{ string_compare()
int string_compare(const void *a, const void *b) 
{ 
   string_t *_a = (string_t*) a;
   string_t *_b = (string_t*) b;

   return strcmp(_a->str, _b->str);   
}
// }}}

// {{{ string_compare_reverse()
int string_compare_reverse(const void *a, const void *b) 
{ 
   string_t *_a = (string_t*) a;
   string_t *_b = (string_t*) b;

   if(strcmp(_a->str, _b->str) > 0)
      return -1;

   if(strcmp(_a->str, _b->str) < 0)
      return  1;
    
   return 0;
}
// }}}



/* ---------------------------------------------------------------------------
 *   String Vector Data Type
 * ------------------------------------------------------------------------ */

// {{{ string_vector_alloc()
string_vector_t *string_vector_alloc()
{
   string_vector_t *sv = malloc(sizeof(string_vector_t));
   sv->size = 0;
   sv->v = NULL;

   return sv;
}
// }}}

// {{{ string_vector_free()
void string_vector_free(string_vector_t *sv)
{
   int i;

   if(!sv)
      return;

   for(i=0; i<sv->size; i++)
      string_free(sv->v[i]);

   free(sv);
}
// }}}

// {{{ string_vector_add()
void string_vector_add(string_vector_t *vs, string_t *o)
{
   if(!vs)
      return;

   vs->size++;
   vs->v = realloc(vs->v, (vs->size)*sizeof(string_t*));
   vs->v[vs->size-1] = o;
}
// }}}




// {{{ string_split()
void string_split(string_vector_t *sv, const char *str, const char *token)
{
   if(!sv)
      return;

   char tmp[strlen(str)+1];
   snprintf(tmp, sizeof(tmp), "%s", str); 

   char *pb = tmp;
   char *pe = tmp;

   while((pe=strstr(pe, token)))
   {
      *pe = 0;
      string_vector_add(sv, string_alloc(pb));

      pe = pe + strlen(token);
      pb = pe;
   }

   string_vector_add(sv, string_alloc(pb));

}
// }}}

// {{{ string_sort()
void string_sort(string_vector_t *sv, int reverse)
{
   if(reverse)
      qsort(sv->v, sv->size, sizeof(char*), string_compare_reverse); 
   else
      qsort(sv->v, sv->size, sizeof(char*), string_compare); 

}
// }}}

// {{{ string_regex_match()
int string_regex_match(string_t *o,  const char *regex)
{
   if(!o)
      return 0;

   int r;
   regex_t expr;
   regmatch_t rm;

   if( (r=regcomp(&expr, regex, REG_EXTENDED))!= 0 ) 
      return 0;
   
   if( (r=regexec(&expr, o->str, 0, &rm, 0))!=0 )
   {
      regfree(&expr);
      return 0;
   }

   regfree(&expr);
   return 1;
}
// }}}

// {{{ string_regex()
void string_regex(string_vector_t *sv, const char *string, const char *regex)
{
   if(!sv)
      return;

   int len = 100; /* Max results */
   int r;
   regex_t expr;
   regmatch_t rm[len];
   int pos=0;

   if( (r=regcomp(&expr, regex, REG_EXTENDED))!= 0 ) 
      return;
   
   if( (r=regexec(&expr, string, len, rm, 0))!=0 )
   {
      regfree(&expr);
      return;
   }

   for(pos=0; pos<len; pos++)
   {
      if(rm[pos].rm_so<0)
      {
         return;
      }

      if(rm[pos].rm_so>=0)
      {
         char result[rm[pos].rm_eo-rm[pos].rm_so+1];
         strncpy(result, &string[rm[pos].rm_so], rm[pos].rm_eo-rm[pos].rm_so+1);
         result[rm[pos].rm_eo-rm[pos].rm_so]=0;
         string_vector_add(sv, string_alloc(result));
      }
   }

   regfree(&expr);
   return;
}
// }}}


