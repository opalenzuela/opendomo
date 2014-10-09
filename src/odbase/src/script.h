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


#ifndef __SCRIPT_H__
#define __SCRIPT_H__

#define MAX_ENV_SIZE 256

typedef struct
{
   char user[MAX_ENV_SIZE];
   char agent_address[MAX_ENV_SIZE];
}
script_env_t;




int script_exec(const char* cmd, const char* section, script_env_t *env);

int split_line_to_cells(	
	const char *linia, 
	char *col1, size_t col1_len,
	char *col2, size_t col2_len,
	char *col3, size_t col3_len,
	char *col4, size_t col4_len,
	char *col5, size_t col5_len);


#endif




