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


/* 
 * author: Daniel Lerch <dlerch@gmail.com>, http://opendomo.org
 * date: February 2010
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>

#include "printlog.h"

#define EVCMD_D "/usr/bin/logger -p odhal.debug "
#define EVCMD_N "/bin/logevent notice odhal "
#define EVCMD_W "/bin/logevent warning odhal "
#define EVCMD_A "/bin/logevent alarm odhal "
#define EVCMD_E "/bin/logevent error odhal "

int printlog(int type, const char* format, ...) 
{
   va_list l;
   int res = -1;
	char msg[64];
	char cmd[128];



	va_start(l, format);
	res = vsnprintf(msg, sizeof(msg), format, l);
	va_end(l);


	//if(!str_regex_match(msg, "REGEX"))
	//	return res;
	
	// TODO: possible code injection security bug



	switch(type)
	{
		case LOG_D:
		res = snprintf(cmd, sizeof(cmd), EVCMD_D " '%s'", msg);
		break;

		case LOG_N:
		res = snprintf(cmd, sizeof(cmd), EVCMD_N " '%s'", msg);
		break;

		case LOG_W:
		res = snprintf(cmd, sizeof(cmd), EVCMD_W " '%s'", msg);
		break;

		case LOG_A:
		res = snprintf(cmd, sizeof(cmd), EVCMD_A " '%s'", msg);
		break;

		case LOG_E:
		res = snprintf(cmd, sizeof(cmd), EVCMD_E " '%s'", msg);
		break;
	
		default:
		res = snprintf(cmd, sizeof(cmd), EVCMD_N " '%s'", msg);
	}

	system(cmd);

   return res;
}


