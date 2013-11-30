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
#include<coelacanth/queue.h>

TYPEDEF_QUEUE(qint, int);
TYPEDEF_QUEUE(qfloat, float);
TYPEDEF_QUEUE(qstr, char*);

int main()
{
   qint_t *qi = qint_create();

	qint_enqueue(qi, 1);
	qint_enqueue(qi, 2);
	qint_enqueue(qi, 3);
	qint_enqueue(qi, 4);

	while(qint_size(qi)>0)
	{
		printf("%d\n", qint_get_head(qi));
		qint_dequeue(qi);
	}

   qint_free(qi);


   qstr_t *qs = qstr_create();

	qstr_enqueue(qs, strdup("uoa"));
	qstr_enqueue(qs, strdup("aoa"));
	qstr_enqueue(qs, strdup("ooo"));

	while(qstr_size(qs)>0)
	{
		printf("%s\n", qstr_get_head(qs));
		qstr_dequeue(qs);
	}

   qstr_free(qs);

 

   return 0;
}




