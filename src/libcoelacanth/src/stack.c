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
#include<string.h>
#include<coelacanth/stack.h>


TYPEDEF_STACK(istack, int);
TYPEDEF_STACK(fstack, float);
TYPEDEF_STACK(strstack, char*);



int main()
{	
	istack_t *is = istack_create();

	istack_push(is, 1);
	istack_push(is, 2);
	istack_push(is, 3);

	while(istack_size(is)>0)
	{
		printf("%d\n", istack_get(is));
		istack_pop(is);
	}

	istack_free(is);


	strstack_t *ss = strstack_create();

	strstack_push(ss, strdup("bla"));
	strstack_push(ss, strdup("bli"));
	strstack_push(ss, strdup("ooo"));
	
	while(strstack_size(ss)>0)
	{
		printf("%s\n", strstack_get(ss));
		strstack_pop(ss);
	}

	strstack_free(ss);

   return 0;
}


