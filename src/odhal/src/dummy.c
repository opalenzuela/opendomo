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
 * date: August 2009
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

#include <src/str.h>


#include "fifo.h"
#include "rmfr.h"
#include "printlog.h"

#define DRIVER_PATH 	"/var/opendomo/control"
#define CONF_PATH 	"/etc/opendomo/control"
#define PATH_LEN 64

char basedir[64] = "";


typedef struct dummy_data_t
{
   int input_to_read;
   int digital_output_to_write;
	char path[PATH_LEN];
}
dummy_data_t;


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

// {{{ dummy_get_analogic_input()
void dummy_get_analogic_input(char *buff, size_t bufflen, void *udata)
{
   dummy_data_t *dd = (dummy_data_t*) udata;

	FILE *random_file = fopen("/dev/urandom","r");
	char random_seed = 0;
	if(random_file) random_seed = getc(random_file);
	else random_seed = time(NULL);
	srand(random_seed);   
	fclose(random_file);

   /* dummy driver, dummy value ;) 
      in a real driver, you must be query hardware */
   int value = 19+rand() % 5;
	char str_value[11];
	snprintf(str_value, sizeof(str_value), "+%04d.0000", value);

   switch(dd->input_to_read)
   {
      case 1:
      /* read first input */
      snprintf(buff, bufflen, "%s\n", str_value);
      break;

      case 2:
      /* read second input */
      snprintf(buff, bufflen, "%s\n", str_value);
      break;

      default:
      snprintf(buff, bufflen, "off\n");
      break;
   }

}
// }}}

// {{{ dummy_get_digital_input()
void dummy_get_digital_input(char *buff, size_t bufflen, void *udata)
{
   dummy_data_t *dd = (dummy_data_t*) udata;

	FILE *random_file = fopen("/dev/urandom","r");
	char random_seed = 0;
	if(random_file) random_seed = getc(random_file);
	else random_seed = time(NULL);
	srand(random_seed);   
	fclose(random_file);
  
   /* dummy driver, dummy value ;) 
      in a real driver, you must be query hardware */
   int value = rand() % 2;
   char *str_value = NULL;

   if(value)
      str_value = "on";
   else
      str_value = "off";

   switch(dd->input_to_read)
   {
      case 1:
      /* read first input */
      snprintf(buff, bufflen, "%s\n", str_value);
      break;

      case 2:
      /* read second input */
      snprintf(buff, bufflen, "%s\n", str_value);
      break;

      default:
      snprintf(buff, bufflen, "off\n");
      break;
   }

}
// }}}

// {{{ dummy_set_digital_output()
void dummy_set_digital_output(char *buff, size_t bufflen, void *udata)
{
   dummy_data_t *dd = (dummy_data_t*) udata;
   openlog("dummy", LOG_PID, LOG_USER);  

	printlog(LOG_N, "set output [%d] to [%s]\n", 
		dd->digital_output_to_write, buff);

	create_output_value_file(dd->path, buff);

   switch(dd->digital_output_to_write)
   {
      case 1:
      /* write first input */
      /* Assign buff value to hardware */
      break;

      case 2:
      /* write second input */
      /* Assign buff value to hardware */
      break;

      default:
      /* Assign buff value to hardware */
      break;
   }

}
// }}}

// {{{ create_output_info_file()
void create_output_info_file(const char *file)
{
   char filename[64];
   snprintf(filename, sizeof(filename), "%s.info", file);

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
   fprintf(f, "status=enabled\n");
   fclose(f);
   
   chmod(filename, 0444);
   chown(filename,1000,1000);
}
// }}}

// {{{ create_input_info_file()
void create_input_info_file(const char *file)
{
   char filename[64];
   snprintf(filename, sizeof(filename), "%s.info", file);

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
   fprintf(f, "status=enabled\n");
   fclose(f);
   chown(filename,1000,1000);
   chmod(filename, 0444);
}
// }}}

void usage(const char* prog)
{
   printf("%s -d [device] -p [dirname] -r [refresh interval] \n", prog);
   exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
   int status;
   char c;

   char device[64] = "";
   char dirname[64] = "";
   char refresh[8] = "";
   char di01[64] = "";
   char di02[64] = "";
   char do01[64] = "";
   char do02[64] = "";
   char ai01[64] = "";
   char ai02[64] = "";
   char idi01[64] = "";
   char idi02[64] = "";
   char ido01[64] = "";
   char ido02[64] = "";
   char iai01[64] = "";
   char iai02[64] = "";

   /* dummy params, not used */
   while((c = getopt(argc, argv, "d:p:r:"))>=0)
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
      }
   }

   if(c==-1 && (strlen(device)==0 ||strlen(dirname)==0||strlen(refresh)==0))
      usage(argv[0]);


   // Initialize random generator
   srand(time(NULL));


   // Replace dangerous chars 
   // TODO: use regex
   str_replace(dirname, sizeof(dirname), ".", "_");
   str_replace(dirname, sizeof(dirname), "/", "_");

   // Configure paths
   snprintf(basedir, sizeof(basedir), DRIVER_PATH "/%s", dirname);
   snprintf(di01, sizeof(di01), DRIVER_PATH "/%s/di001", dirname);
   snprintf(di02, sizeof(di02), DRIVER_PATH "/%s/di002", dirname);
   snprintf(do01, sizeof(do01), DRIVER_PATH "/%s/do001", dirname);
   snprintf(do02, sizeof(do02), DRIVER_PATH "/%s/do002", dirname);
   snprintf(ai01, sizeof(ai01), DRIVER_PATH "/%s/ai001", dirname);
   snprintf(ai02, sizeof(ai02), DRIVER_PATH "/%s/ai002", dirname);
   snprintf(idi01, sizeof(idi01), CONF_PATH "/%s/di001", dirname);
   snprintf(idi02, sizeof(idi02), CONF_PATH "/%s/di002", dirname);
   snprintf(ido01, sizeof(ido01), CONF_PATH "/%s/do001", dirname);
   snprintf(ido02, sizeof(ido02), CONF_PATH "/%s/do002", dirname);
   snprintf(iai01, sizeof(iai01), CONF_PATH "/%s/ai001", dirname);
   snprintf(iai02, sizeof(iai02), CONF_PATH "/%s/ai002", dirname);

   // Remove old dirs
   //rmfr(basedir);

   // Create working dir
   mkdir(basedir, 0755);
   chown(basedir,1000,1000);


   dummy_data_t dd;

   dd.input_to_read = 1;
   fifo_input(di01, dummy_get_digital_input, &dd);
   create_input_info_file(idi01);

   dd.input_to_read = 2;
   fifo_input(di02, dummy_get_digital_input, &dd);
   create_input_info_file(idi02);

   dd.input_to_read = 1;
   fifo_input(ai01, dummy_get_analogic_input, &dd);
   create_input_info_file(iai01);

   dd.input_to_read = 2;
   fifo_input(ai02, dummy_get_analogic_input, &dd);
   create_input_info_file(iai02);



   dd.digital_output_to_write = 1;
	snprintf(dd.path, PATH_LEN, "%s", do01);
   fifo_output(do01, dummy_set_digital_output, &dd);
   create_output_info_file(ido01);
	create_output_value_file(dd.path, "off");

   dd.digital_output_to_write = 2;
	snprintf(dd.path, PATH_LEN, "%s", do02);
   fifo_output(do02, dummy_set_digital_output, &dd);
   create_output_info_file(ido02);
	create_output_value_file(dd.path, "off");


   wait(&status);
   printf("status: %d\n", status);

   return 0;
} 


