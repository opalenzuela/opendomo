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


#ifndef __FILE_H__
#define __FILE_H__

/** file_t data type */
typedef struct
{
   char filename[256];
   char error[256];
}
file_t;


void file_set_filename(file_t *o, const char *filename);

int file_is_file(file_t *o);

int file_is_dir(file_t *o);

void file_print(file_t *o);

int file_get(file_t *o, char * out, int len);

int file_error(file_t *o);

const char* file_get_error(file_t *o);




#endif




