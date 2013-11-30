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
/** 
  @file mjpg.c
  @brief TCP tunneling functions
 */

#include<stdio.h>
#include<unistd.h>
#include<netdb.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdlib.h>
#include<syslog.h>
#include<errno.h>
#include<fcntl.h>

//#include "perceiving/vision/jpeg.h"

#include "coelacanth/str.h"
#include "coelacanth/string.h"
#include "ai/perceiving/vision/mjpg.h"

#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>


#include "mjpg.h"


#define RESOURCE_PATH "/etc/opendomo/video/"


// {{{ mjpg_streaming_rsc()
int mjpg_streaming_rsc(const char *resource)
{
	char line[128];
	char varname[128];
	char varcontent[128];
	char filename[128];
	char url[256];
	char auth[64];
	FILE *f;

	// Check if video access is enabled
	f = fopen("/var/run/odvision_access.pid","r");
	if (!f)
	{
		syslog(LOG_WARNING, "Access denied to cameras\n");
		return -1;
	}
	fclose(f);	

	snprintf(filename, sizeof(filename), "%s%s.conf", RESOURCE_PATH, resource);

	f = fopen(filename, "r");
	if(!f)
	{
		syslog(LOG_WARNING, "Cannot open resource (file): %s\n", filename);
		return -1;
	}

	// Parse file
	while(!feof(f))
	{
		fgets(line, sizeof(line), f);
		char *ptr = str_split(line, "=", varname, sizeof(varname));
		if(ptr)
		{
			str_trim(varname);
			snprintf(varcontent, sizeof(varcontent), "%s", ptr);
			str_replace(varcontent, sizeof(varcontent), "\"", "");
			str_trim(varcontent);

			if(strcmp(varname, "URL")==0)
				snprintf(url, sizeof(url), "%s", varcontent);

			else if(strcmp(varname, "AUTH")==0)
				snprintf(auth, sizeof(auth), "%s", varcontent);
		}
	}

	fclose(f);

	// Parse URL
	char ip[32];
	char path[256];
	int port;

	string_vector_t *mres = string_vector_alloc();

   string_regex(mres, url, "http://([^:/]+)([:0-9]*)(.*)");

	if(mres->size==4)
	{
		snprintf(ip, sizeof(ip), "%s", mres->v[1]->str);
		port = atoi(mres->v[2]->str+1);
		if(port==0)
			port=80;
		snprintf(path, sizeof(path), "%s", mres->v[3]->str);
	}
	else	
	{
		syslog(LOG_WARNING, "URL format error\n");
   	string_vector_free(mres);
		return -1;
	}

   string_vector_free(mres);

	/*
   syslog(LOG_WARNING, "<br> ip: %s\n", ip);
   syslog(LOG_WARNING, "<br> url: %s\n", path);
   syslog(LOG_WARNING, "<br> port: %d\n", port);
   syslog(LOG_WARNING, "<br> cred: %s\n", auth);
	*/
	// Parse URL
	mjpg_t p;
	p.url = path;
	snprintf(p.host, sizeof(p.host), "%s", ip);
	p.auth = auth;
	p.base = NULL;
	p.port = port;
	p.threshold = 0;
	p.flags = M_STDOUT | M_DRAW_SHAPE | M_READ_FROM_SOCKET;
	memset(p.zones, 0, sizeof(p.zones));	
	return mjpg_streaming(&p);
}
// }}}

// {{{ mjpg_play()
void mjpg_play(const char* file)
{
	
	char cffile[128];
	char line[128];
	char varname[128];
	char varcontent[128];
	char target_dir[128];
	struct stat statbuf;
	snprintf(cffile, sizeof(cffile), RESOURCE_PATH "common.conf");


	FILE *f = fopen(cffile, "r");
	if(!f)
	{
		syslog(LOG_WARNING, "Cannot open resource (file): %s\n", cffile);
		return;
	}

	// Parse file
	while(!feof(f))
	{
		fgets(line, sizeof(line), f);
		char *ptr = str_split(line, "=", varname, sizeof(varname));
		if(ptr)
		{
			str_trim(varname);
			snprintf(varcontent, sizeof(varcontent), "%s", ptr);
			str_replace(varcontent, sizeof(varcontent), "\"", "");
			str_trim(varcontent);

			if(strcmp(varname, "TARGET_DIR")==0)
			{
				snprintf(target_dir, sizeof(target_dir), "%s", varcontent);
				break;
			}

		}
	}

	fclose(f);


	char filename[128];
	char pfile[128];
	off_t position = 0;
	int stop=0;

	char *pos = str_split(file, ":", pfile, sizeof(pfile));
	if(pos)
	{
		if(pos[0]=='s')
		{
			stop=1;
			pos++;
		}

		position = atoll(pos);
	}

	snprintf(filename, sizeof(filename), "%s/%s.mjpg", target_dir, pfile);
	syslog(LOG_NOTICE, "filename: %s\n", filename);


	mjpg_t p;
	snprintf(p.src_file, sizeof(p.src_file), "%s", filename);
	p.offset = position;
	p.offset_stop = stop;
	p.flags = M_STDOUT | M_DRAW_SHAPE | M_READ_FROM_FILE | M_OFFSET;
	mjpg_streaming(&p);


}
// }}}

