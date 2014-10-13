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
#include<unistd.h>
#include<netdb.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdlib.h>
#include<syslog.h>
#include<errno.h>
#include<fcntl.h>


#define	M_STDOUT					0x00000001	// 00000001
#define 	M_RECORD 				0x00000002	// 00000010
#define 	M_RECORD_THRESHOLD 	0x00000004	// 00000100
#define	M_DRAW_SHAPE 			0x00000008	// 00001000
#define 	M_READ_FROM_SOCKET 	0x00000010	// 00010000
#define 	M_READ_FROM_FILE		0x00000020	// 00100000
#define 	M_OFFSET 				0x00000040	// 01000000
#define M_1 0x00000080	// 10000000


#include "src/jpeg.h"
#include "src/base64.h"

#include "src/str.h"
#include "src/string.h"

/* mjpg data type */
typedef struct mjpg_t
{

	/* The following values are used with M_READ_FROM_SOCKET flag, and 
      specify the source of streaming */	
	char host[32];
	char path[256];
	int port;
	char *url;
	char *auth;
	char *base;



	zone_t zones[MAX_ZONES];



	/* Used with flag M_RECORD_THRESHOLD and indicate the threshold value
      to start recording */ 
	int threshold;
	
	/* Used with flag M_READ_FROM_FILE. Specifies the source file name */
	char src_file[64];

	/* Used with flag M_OFFSET and M_READ_FROM_FILE. 
		Jump to offset position, where offset is a percentage of total 
		image length */
	off_t offset;

	/* If offset_stop is true the streaming must be stopped in the first frame */
	short offset_stop;

	/* Contains flags that regulate the behavior of the funcion */
	int flags;

} mjpg_t;


/* if threshold==0 always save video.
	if base is NULL, write video in the standard output
 */
/*int mjpg_streaming(const char *host, const char *url, size_t port, 
	const char *credentials, int threshold, const char *base, int flags);*/

int mjpg_streaming(mjpg_t *o);


int mjpg_verify(const char *host, const char *url, size_t port, 
	const char *credentials);


int mjpg_cam_covered(const char *host, const char *url, size_t port, 
					 const char *credentials, int threshold, int cnt_imgs);




