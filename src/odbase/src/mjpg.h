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


#ifndef __TUNNEL_H__
#define __TUNNEL_H__


/* Connect to HTTP straming source and build a tunnel with client. 
   Tested only with motion.
   Returns 0 if ok, -1 on error

example:
tunnel_http_streaming("192.168.1.151", "/mjpg/video.mjpg", 80, "root:pass", 1);	
*/
int tunnel_http_streaming(
	const char *host, 
	const char *url, 
	size_t port, 
	const char *credentials, 
	size_t frmmult);



int mjpg_streaming_rsc(const char *resource);

void mjpg_play(const char* file);

int tunnel_http_streaming_rsc(const char *resource);

#endif




