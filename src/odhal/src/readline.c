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
 * date: January 2010
 */

#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<ctype.h>


int readline(char *line, size_t len, int fd)
{
	char c;
	int i=0;
	int n;
	while(i<len)
	{
		n=read(fd, &c, 1);
		if(n==-1)
		{
			if(errno==EINTR)
				continue;
			return -1;			
		}
		else if(n==0)	
		{
			usleep(100000);
			continue;
		}

		if(c=='\n')
			break;


		if(!isascii(c)) c=' ';

		line[i++]=c;
	}

	line[i]=0;
	return i;
}




