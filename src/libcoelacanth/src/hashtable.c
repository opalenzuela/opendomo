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
#include <coelacanth/hashtable.h>

TYPEDEF_HASHTABLE(htint, int);
TYPEDEF_HASHTABLE(htfloat, float);
TYPEDEF_HASHTABLE(htstr, char*);




int main()
{
	htint_t *hi = htint_create(10, 0);

	char key[10];
	strcpy(key, "bla");

	htint_set(hi, key, 5);
	htint_set(hi, "ble", 6);
	htint_set(hi, "bli", 6);

	printf("%d\n", htint_get(hi, "bla"));
	printf("%d\n", htint_get(hi, "ble"));
	printf("%d\n", htint_get(hi, "bli"));
	printf("%d\n", htint_get(hi, "ooops"));


	htint_free(hi);


	htstr_t *hs = htstr_create(10, "");

	htstr_set(hs, "bla", strdup("ai"));
	htstr_set(hs, "ble", strdup("ei"));
	htstr_set(hs, "bli", strdup("oi"));

	free(htstr_get(hs, "bla"));
	htstr_set(hs, "bla", strdup("uu"));

	printf("%s\n", htstr_get(hs, "bla"));
	printf("%s\n", htstr_get(hs, "ble"));
	printf("%s\n", htstr_get(hs, "bli"));
	printf("%s\n", htstr_get(hs, "ooops"));

	htstr_free(hs);



   return 0;
}




