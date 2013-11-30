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
 * date: August 2011
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>

#include <coelacanth/str.h>


#include "x10lib.h"
#include "fifo.h"
#include "rmfr.h"
#include "printlog.h"

#define DRIVER_PATH	"/var/opendomo/control"
#define CONF_PATH  	"/etc/opendomo/control/cm15X10"
#define PATH_LEN 64
#define NAME_LEN 16

char basedir[64] = "";


typedef struct x10_data_t
{
	int devid;
	char devname[NAME_LEN];
	char path[PATH_LEN];
}
x10_data_t;


// {{{ create_output_value_file()
void create_output_value_file(const char *path, const char *value)
{
   char filename[64];

   snprintf(filename, sizeof(filename), "%s.value", path);

   unlink(filename);
   FILE *f = fopen(filename, "w+");
   if(!f)
   {
		printlog(LOG_E, "create_output_value_file() - fopen(): %s : %s",
         path, strerror(errno));

      return;
   }

	char v[strlen(value)+1];
	snprintf(v, sizeof(v), "%s", value);
	str_to_lower(v, sizeof(v));

   fprintf(f, "%s\n", v);
   fclose(f);
   
   chmod(filename, 0444);
   chown(filename,1000,1000);
}
// }}}

// {{{ x10_get_digital_input()
void x10_get_digital_input(char *buff, size_t bufflen, void *udata)
{
   x10_data_t *xd = (x10_data_t*) udata;

	int value = 0;
   snprintf(buff, bufflen, "%d\n", value);

}
// }}}

// {{{ x10_set_digital_output()
void x10_set_digital_output(char *buff, size_t bufflen, void *udata)
{
	char cmd[32];
   x10_data_t *xd = (x10_data_t*) udata;
   openlog("x10", LOG_PID, LOG_USER);  

	printlog(LOG_N, "set output [%s] to [%s]\n", 
		xd->devname, buff);

   /* Assign buff value to hardware */
	if(strcasecmp(buff, "on")==0 || strcmp(buff, "1")==0)
	{
		snprintf(cmd, sizeof(cmd), "%s on", xd->devname);
   	create_output_value_file(xd->path, "on");
	}

	if(strcasecmp(buff, "off")==0 || strcmp(buff, "0")==0)
	{
		snprintf(cmd, sizeof(cmd), "%s off", xd->devname);
   	create_output_value_file(xd->path, "off");
	}

	// x10 initialization
	if(x10_init()!=0)
	{
		fprintf(stderr, "Error: x10_init()\n");
		return;
	}

	x10_send_command(xd->devid, cmd);
	x10_release();
}
// }}}

// {{{ create_output_info_file()
void create_output_info_file(const char *file)
{
   char filename[64];
   snprintf(filename, sizeof(filename), CONF_PATH "/XX%s.info", file);

	FILE *f;
	// Try to open the file for reading
	f = fopen(filename,"r");
	if (f != NULL) {
		// If the file exists, abort
		fclose(f);
		return;
    } 

   f = fopen(filename, "w+");
   if(!f)
   {
		printlog(LOG_E, "fopen() : create_output_info_file() : %s : %s\n", 
         file, strerror(errno));
      return;
   }
   fprintf(f, "way=out\n");
   fprintf(f, "values=on,off\n");
   fprintf(f, "status=disabled\n");
   fclose(f);
   
   chmod(filename, 0444);
   chown(filename,1000,1000);
}
// }}}

// {{{ create_input_info_file()
void create_input_info_file(const char *file)
{
   char filename[64];
   snprintf(filename, sizeof(filename), CONF_PATH "/XX%s.info", file);

	FILE *f;
	// Try to open the file for reading
	f = fopen(filename,"r");
	if (f != NULL) {
		// If the file exists, abort
		fclose(f);
		return;
    } 

   f = fopen(filename, "w+");
   if(!f)
   {
		printlog(LOG_E, "fopen() : create_input_info_file() : %s : %s\n", 
         file, strerror(errno));
      return;
   }
   fprintf(f, "way=in\n");
   fprintf(f, "status=disabled\n");
   fclose(f);
   chown(filename,1000,1000);
   chmod(filename, 0444);
}
// }}}

void usage(const char* prog)
{
   printf("%s -d [device] -p [dirname] -r [refresh interval] \n", prog);
   printf("%s -l\n", prog);
   exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
   int status;
   char c;

   char device[64] = "";
   char dirname[64] = "";
   char refresh[8] = "";


   /* x10 params, not used */
   while((c = getopt(argc, argv, "d:p:r:l"))>=0)
   {
      switch(c)
      {
         case 'd':
            snprintf(device, sizeof(device), "%s", optarg);
            break;

         case 'p':
            snprintf(dirname, sizeof(dirname), "%s", optarg);
            break;

         case 'r':
            snprintf(refresh, sizeof(refresh), "%s", optarg);
            break;

         case 'l':
				if(x10_init()!=0)
				{
					fprintf(stderr, "Error: x10_init()\n");
					exit(EXIT_FAILURE);
				}

				int i;
				for(i=1; i<=x10_dev_idx; i++)	
				{
					printf("x10cm15a%d\n", i);
				}

				x10_release();
				exit(EXIT_SUCCESS);

            break;
      }
   }

   if(c==-1 && (strlen(device)==0 ||strlen(dirname)==0||strlen(refresh)==0))
      usage(argv[0]);


	int devid = 0;
	sscanf(device, "x10cm15a%d", &devid);
	if(devid==0)
	{
		fprintf(stderr, "Incorrect device");
		exit(EXIT_FAILURE);
	}

	// Initialize random generator
	srand(time(NULL));


	// Replace dangerous chars 
	// TODO: use regex
	str_replace(dirname, sizeof(dirname), ".", "_");
	str_replace(dirname, sizeof(dirname), "/", "_");

	// Configure paths
	snprintf(basedir, sizeof(basedir), DRIVER_PATH "/%s", dirname);

	// Create working dir
	rmfr(basedir);
	mkdir(basedir, 0755);
	chown(basedir, 1000,1000);

	// Create config dir
	mkdir(CONF_PATH, 0755);

	// Create output devices
	char housecode[] = "-abcdefghijklmnop";
	int i, j;
	for(i=1; i<=16; i++)
	{
		for(j=1; j<=16; j++)
		{
			x10_data_t xd;
			xd.devid=devid;

			if(j<10)
				snprintf(xd.devname, NAME_LEN, "%c0%d", housecode[i], j); 
			else
				snprintf(xd.devname, NAME_LEN, "%c%d", housecode[i], j); 

			snprintf(xd.path, PATH_LEN, DRIVER_PATH "/%s/XX%s", dirname, xd.devname);
			fifo_output(xd.path, x10_set_digital_output, &xd);

			//char path[PATH_LEN];
			//snprintf(path, PATH_LEN, CONF_PATH "/%s/%s", dirname, xd.devname);
			create_output_info_file(xd.devname);
		}
	}



	/*
   xd.digital_input_to_read = 1;
   fifo_input(di01, x10_get_digital_input, &xd);
   create_input_info_file(di01);

   xd.digital_input_to_read = 2;
   fifo_input(di02, x10_get_digital_input, &xd);
   create_input_info_file(di02);
	*/

   wait(&status);
   printf("status: %d\n", status);

   return 0;
} 


