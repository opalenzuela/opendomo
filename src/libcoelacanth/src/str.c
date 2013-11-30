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


#include <coelacanth/str.h>
#include <coelacanth/vector.h>

int main()
{
   char buffer[16] = "";

   str_assign_int(buffer, sizeof(buffer), 2456);
   printf("%s=%d\n", buffer, str_to_int(buffer));

   str_assign_float(buffer, sizeof(buffer), 3.1416);
   printf("%s=%f\n", buffer, str_to_float(buffer));

   str_assign_str(buffer, sizeof(buffer), "  \r\n Hello World!    \r\t\n   ");
   str_trim(buffer);
   printf("%s\n", buffer);

   str_cut(buffer, sizeof(buffer), 19, 25);
   printf("%s\n", buffer);

   str_left(buffer, sizeof(buffer), 5);
   printf("%s\n", buffer);

   str_right(buffer, sizeof(buffer), 2);
   printf("%s\n", buffer);

   str_assign_str(buffer, sizeof(buffer), "one two three");
   printf("pattern at: %d\n", str_find(buffer, "two"));

   if(str_starts_with(buffer, "one"))
      puts("starts ok!");

   if(str_ends_with(buffer, "three"))
      puts("ends ok!");

   str_assign_str(buffer, sizeof(buffer), "1234");
   printf("%s\n", buffer);

   str_lpad(buffer, sizeof(buffer), '0', 7);
   printf("%s\n", buffer);

   str_rpad(buffer, sizeof(buffer), '9', 10);
   printf("%s\n", buffer);


   char *str = "root:x:0:0:root:/root:/bin/bash";

   while((str = str_split(str, ":", buffer, sizeof(buffer))))
      printf("%s\n", buffer);
   printf("%s\n", buffer);

   str = "hi-|-how";
   str = str_split(str, "-|-", buffer, sizeof(buffer));
   printf("%s\n", buffer);
   str = str_split(str, "-|-", buffer, sizeof(buffer));
   printf("%s\n", buffer);



   snprintf(buffer, sizeof(buffer), "hola que tal");
   str_replace(buffer, sizeof(buffer), "a", "A");
   printf("%s\n", buffer);
   str_replace(buffer, sizeof(buffer), "A", "OOO");
   printf("%s\n", buffer);
   str_replace(buffer, sizeof(buffer), "o", "XXXXXXXXXXXXXXXXX");
   printf("%s\n", buffer);
 
   if(str_regex_match("holas1", "[a-z0-9]"))
      puts("match!");
 
	snprintf(buffer, sizeof(buffer), "holaS");
	str_to_upper(buffer, sizeof(buffer));
	printf("upper: %s\n", buffer);

	snprintf(buffer, sizeof(buffer), "HOLAs");
	str_to_lower(buffer, sizeof(buffer));
	printf("lower: %s\n", buffer);
 
	int r = str_random(buffer, sizeof(buffer), 15, "abc123");
	printf("random(%d): %s\n", r, buffer);

   return 0;
}


