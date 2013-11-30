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
#include<coelacanth/string.h>

void addstrings(string_vector_t *sv)
{
   string_vector_add(sv, string_alloc("D"));
   string_vector_add(sv, string_alloc("E"));
}


int main()
{
   int i;

   string_t *s = string_alloc("");

   string_assign_int(s, 2456);
   printf("%s=%d\n", s->str, string_to_int(s));

   string_assign_float(s, 3.1416);
   printf("%s=%f\n", s->str, string_to_float(s));

   string_assign_str(s, "  \r\n Hello World!    \r\t\n   ");
   string_trim(s);
   printf("%s\n", s->str);

   string_replace(s, "He", "Heeeeeeeeeeeeee");
   printf("%s\n", s->str);

   string_append(s, " Again!");
   printf("%s\n", s->str);

   string_cut(s, 19, 25);
   printf("%s\n", s->str);

   string_left(s, 5);
   printf("%s\n", s->str);

   string_right(s, 2);
   printf("%s\n", s->str);

   string_assign_str(s, "one two three");
   printf("pattern at: %d\n", string_find(s, "two"));

   if(string_starts_with(s, "one"))
      puts("starts ok!");

   if(string_ends_with(s, "three"))
      puts("ends ok!");

   string_assign_str(s, "1234");
   printf("%s\n", s->str);

   string_lpad(s, '0', 7);
   printf("%s\n", s->str);

   string_rpad(s, '9', 10);
   printf("%s\n", s->str);

   string_assign_str(s, "hi_ho_23");
   printf("%s\n", s->str);

   if(string_regex_match(s, "[a-z0-9_]+"))
      puts("match ok!");
   
   string_vector_t *mres = string_vector_alloc();

   string_regex(mres, "Nombre: Daniel, edad: 31", 
      "[a-zA-Z]+: ([a-zA-Z]+), edad: ([0-9]+)");
   for(i=0; i<mres->size; i++)
      printf("mres[%d]=%s\n", i, mres->v[i]->str);

   string_vector_free(mres);



   string_vector_t *vs = string_vector_alloc();
   char *str = "root:x:0:0:root:/root:/bin/bash";

   string_split(vs, str, ":");
   for(i=0; i<vs->size; i++)
      printf("vs[%d]=%s\n", i, vs->v[i]->str);   


   string_vector_free(vs);

   string_free(s);



   string_vector_t *sv = string_vector_alloc();

   string_vector_add(sv, string_alloc("C"));
   string_vector_add(sv, string_alloc("B"));
   string_vector_add(sv, string_alloc("A"));

   addstrings(sv);

   string_sort(sv, 1);

   for(i=0; i<sv->size; i++)
      printf("vector[%d]=%s\n", i, sv->v[i]->str);

   string_vector_free(sv);


   return 0;
}


