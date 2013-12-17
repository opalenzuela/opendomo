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
 * date: October 2009
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
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/mman.h>




#include <src/str.h>

#include "fifo.h"
#include "rmfr.h"
#include "semaphore.h"
#include "printlog.h"

#define BAUDRATE B9600
#define DRIVER_PATH "/var/opendomo/control"
#define PULSE_INTERVAL 100000
#define PATH_LEN 64

/* shared id */
int semid = 0;
int shmid = 0;

/* mmaped data */
int w_data_fd = 0;
unsigned int *w_data = NULL;

typedef struct ps3x8_data_t
{
   int device;
   int digital_input_to_read;
   int digital_output_to_write;
	char path[PATH_LEN];
}
ps3x8_data_t;


/* ps3x8 Functions */

// {{{ Read input state 
#define PS3X8_GET_IO1(data) (data & 0x01 ? 1 : 0)
#define PS3X8_GET_IO2(data) (data & 0x02 ? 1 : 0)
#define PS3X8_GET_IO3(data) (data & 0x04 ? 1 : 0)
#define PS3X8_GET_IO4(data) (data & 0x08 ? 1 : 0)
#define PS3X8_GET_IO5(data) (data & 0x10 ? 1 : 0)
#define PS3X8_GET_IO6(data) (data & 0x20 ? 1 : 0)
#define PS3X8_GET_IO7(data) (data & 0x40 ? 1 : 0)
#define PS3X8_GET_IO8(data) (data & 0x80 ? 1 : 0)
// }}}

// {{{ Set outputs 
#define PS3X8_SET_IO1(value,data) if(value) data |= 0x01; else data &= ~0x01;
#define PS3X8_SET_IO2(value,data) if(value) data |= 0x02; else data &= ~0x02;
#define PS3X8_SET_IO3(value,data) if(value) data |= 0x04; else data &= ~0x04;
#define PS3X8_SET_IO4(value,data) if(value) data |= 0x08; else data &= ~0x08;
#define PS3X8_SET_IO5(value,data) if(value) data |= 0x10; else data &= ~0x10;
#define PS3X8_SET_IO6(value,data) if(value) data |= 0x20; else data &= ~0x20;
#define PS3X8_SET_IO7(value,data) if(value) data |= 0x40; else data &= ~0x40;
#define PS3X8_SET_IO8(value,data) if(value) data |= 0x80; else data &= ~0x80;
// }}}                

// {{{ ps3x8_connect()
int ps3x8_connect(const char *serial)
{
   int fd;
   fd_set wfds, rfds;
   struct timeval tv;
   struct termios oldtio, newtio;

   if( (fd = open(serial, O_RDWR | O_NOCTTY)) == -1)
   {
		printlog(LOG_E, "ps3x8_connect() - open(): %s", strerror(errno));
      return -1; 
   }

   if(tcgetattr(fd, &oldtio) < 0)
   {
		printlog(LOG_E, "ps3x8_connect() - tcgetattr(): %s", strerror(errno));
      return -1; 
   }

   memset(&newtio, 0, sizeof(newtio));
   newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
   newtio.c_cflag &= ~CSTOPB;
   newtio.c_cflag &= ~PARENB;
   newtio.c_cflag &= ~CRTSCTS;
   newtio.c_iflag = IGNPAR;
   newtio.c_oflag = 0;
   newtio.c_lflag = 0;
   newtio.c_cc[VTIME]    = 0;   // inter-character timer unused 
   newtio.c_cc[VMIN]     = 0;   // blocking read until X chars received 


   if(tcflush(fd, TCIFLUSH) < 0)
   {
		printlog(LOG_E, "ps3x8_connect() - tcflush(): %s", strerror(errno));
      return -1;
   }

   if(tcsetattr(fd, TCSANOW, &newtio) < 0)
   {
		printlog(LOG_E, "ps3x8_connect() - tcsetattr(): %s", strerror(errno));
      return -1;
   }
	

   // -- Escritura --
   FD_ZERO(&wfds);
   FD_SET(fd, &wfds);
   tv.tv_sec = 5;
   tv.tv_usec = 0;

   if(select(fd+1, NULL, &wfds, NULL, &tv))
   { 
      FD_CLR(fd, &wfds);
      unsigned char buf[8];
      buf[0]=0x06;
      buf[1]=0x00;
      if( write(fd, buf, 1) <=0)
      {
			printlog(LOG_E, "ps3x8_connect() - write(): %s", strerror(errno));
         return -1;
      }
   }
   else
   {
		printlog(LOG_E, "ps3x8_connect() - select(): %s", strerror(errno));
      return -1;
   }

   // -- Lectura --
   FD_ZERO(&rfds);
   FD_SET(fd, &rfds);
   tv.tv_sec = 5;
   tv.tv_usec = 0;

   if(select(fd+1, &rfds, NULL, NULL, &tv))
   { 
      FD_CLR(fd, &rfds);
      unsigned char buf[8];
      buf[0]=0;
      if(read(fd, buf, 8) < 0)
      {
			printlog(LOG_E, "ps3x8_connect() - read(): %s", strerror(errno));
         return -1;
      }

      if(buf[0]!=0x06)
      {
			printlog(LOG_E, "ps3x8_connect() - TX error, received != 0x06\n");
         return -1;
      }
   }
   else
   {
		printlog(LOG_E, "ps3x8_connect() - TX error\n");
      return -1;
   }

   return fd;
}
// }}}

// {{{ ps3x8_disconnect()
void ps3x8_disconnect(int fd)
{
   if(close(fd)==-1)
   {
		printlog(LOG_E, "ps3x8_disconnect() - close(): %s", strerror(errno));
   }
}
// }}}

// {{{ ps3x8_read_digital_data()
int ps3x8_read_digital_data(int fd, unsigned int *data)
{
   fd_set wfds, rfds;
   struct timeval tv;

   if(fd==-1)
   {
		printlog(LOG_E, "ps3x8_read_digital_data() - fd == -1 ");
      return -1;
   }

   // -- Escritura --
   FD_ZERO(&wfds);
   FD_SET(fd, &wfds);
   tv.tv_sec = 5;
   tv.tv_usec = 0;

   if (select(fd+1, NULL, &wfds, NULL, &tv))
   { 
      FD_CLR(fd, &wfds);
      unsigned char buf[8];
      buf[0]=0x01;
      buf[1]=0x00;
      if(write(fd, buf, 1) <= 0)
      {
			printlog(LOG_E, "ps3x8_read_digital_data() - write(): %s", 
				strerror(errno));
         return -1;
      }
   }
   else
   {
		printlog(LOG_E, "ps3x8_read_digital_data() - select(): %s", 
				strerror(errno));
      return -1;
   }

   // -- Lectura --
   FD_ZERO(&rfds);
   FD_SET(fd, &rfds);
   tv.tv_sec = 5;
   tv.tv_usec = 0;

   if (select(fd+1, &rfds, NULL, NULL, &tv))
   { 
      FD_CLR(fd, &rfds);
      unsigned char buf[8];
      buf[0]=0;
      if(read(fd, buf, 8) < 0)
      {
			printlog(LOG_E, "ps3x8_read_digital_data() - read(): %s", 
				strerror(errno));
         return -1;
      }

      *data = buf[0];
   }
   else 
   {
		printlog(LOG_E, "ps3x8_read_digital_data() - select(): %s", 
			strerror(errno));
   }

   return 0;
}
// }}}

// {{{ ps3x8_write_digital_data()
int ps3x8_write_digital_data(int fd, unsigned int data)
{
   fd_set wfds, rfds;
   struct timeval tv;

   if(fd==-1)
   {
		printlog(LOG_E, "ps3x8_write_digital_data() - fd == -1\n");
      return -1;
   }

   // -- Escritura --
   FD_ZERO(&wfds);
   FD_SET(fd, &wfds);
   tv.tv_sec = 5;
   tv.tv_usec = 0;

   if (select(fd+1, NULL, &wfds, NULL, &tv))
   { 
      FD_CLR(fd, &wfds);
      unsigned char buf[8];
      buf[0]=0x03;
      buf[1]=data;
      buf[2]=0x00;
      if(write(fd, buf, 2) < 0)
      {
			printlog(LOG_E, "ps3x8_write_digital_data() - write(): %s", 
				strerror(errno));
         return -1;
      }
   }
   else
   {
		printlog(LOG_E, "ps3x8_write_digital_data() - select(): %s", 
			strerror(errno));
      return -1;
   }

   // -- Lectura --
   FD_ZERO(&rfds);
   FD_SET(fd, &rfds);
   tv.tv_sec = 5;
   tv.tv_usec = 0;

   if (select(fd+1, &rfds, NULL, NULL, &tv))
   { 
      FD_CLR(fd, &rfds);
      unsigned char buf[8];
      buf[0]=0;
      if(read(fd, buf, 8) < 0)
      {
			printlog(LOG_E, "ps3x8_write_digital_data() - read(): %s", 
				strerror(errno));
         return -1;
      }

      if(buf[0]!=data)
      {
			printlog(LOG_E, "ps3x8_write_digital_data() - RX error");
      }
   }
   else
   {
		printlog(LOG_E, "ps3x8_write_digital_data() - select(): %s", 
			strerror(errno));
      return -1; 
   }

   return 0;
}
// }}}

// {{{ ps3x8_eventlog()
void ps3x8_eventlog(void *udata, int refresh)
{
	pid_t p = 0;

   ps3x8_data_t *pd = (ps3x8_data_t*) udata;
   
	/* Child process */
	if((p=fork())==0)
	{

		while(1)
		{  
			usleep(refresh);      

			static unsigned int data = 0;
			unsigned int tmp = 0;
			int ret = ps3x8_read_digital_data(pd->device, &tmp);
			if(ret!=0)
			{
				printlog(LOG_E, "ps3x8_eventlog() - "
					"ps3x8_read_digital_data() error\n");
				return;
			}

			if(PS3X8_GET_IO1(data)!=PS3X8_GET_IO1(tmp))
				printlog(LOG_N, "IO1=%d", PS3X8_GET_IO1(tmp));

			if(PS3X8_GET_IO2(data)!=PS3X8_GET_IO2(tmp))
				printlog(LOG_N, "IO2=%d", PS3X8_GET_IO2(tmp));

			if(PS3X8_GET_IO3(data)!=PS3X8_GET_IO3(tmp))
				printlog(LOG_N, "IO3=%d", PS3X8_GET_IO3(tmp));

			if(PS3X8_GET_IO4(data)!=PS3X8_GET_IO4(tmp))
				printlog(LOG_N, "IO4=%d", PS3X8_GET_IO4(tmp));

			if(PS3X8_GET_IO5(data)!=PS3X8_GET_IO5(tmp))
				printlog(LOG_N, "IO5=%d", PS3X8_GET_IO5(tmp));

			if(PS3X8_GET_IO6(data)!=PS3X8_GET_IO6(tmp))
				printlog(LOG_N, "IO6=%d", PS3X8_GET_IO6(tmp));

			if(PS3X8_GET_IO7(data)!=PS3X8_GET_IO7(tmp))
				printlog(LOG_N, "IO7=%d", PS3X8_GET_IO7(tmp));

			if(PS3X8_GET_IO8(data)!=PS3X8_GET_IO8(tmp))
				printlog(LOG_N, "IO8=%d", PS3X8_GET_IO8(tmp));
		}

		exit(0);
	}

	/* Error */
	else if(p<0)
	{
		printlog(LOG_E, "ps3x8_write_digital_data() - fork(): %s", 
			strerror(errno));
		return;
	}

	/* Parent */

}
// }}}


/* Template Functions */

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

// {{{ ps3x8_get_digital_input()
void ps3x8_get_digital_input(char *buff, size_t bufflen, void *udata)
{
   ps3x8_data_t *pd = (ps3x8_data_t*) udata;
   
   unsigned int data = 0;
   int ret = ps3x8_read_digital_data(pd->device, &data);
   if(ret!=0)
   {
		printlog(LOG_E, "ps3x8_get_digital_input() - "
			"ps3x8_write_digital_data()");
      return;
   }

   switch(pd->digital_input_to_read)
   {
      case 1:
      snprintf(buff, bufflen, "%d\n", PS3X8_GET_IO1(data));
      break;

      case 2:
      snprintf(buff, bufflen, "%d\n", PS3X8_GET_IO2(data));
      break;

      case 3:
      snprintf(buff, bufflen, "%d\n", PS3X8_GET_IO3(data));
      break;

      case 4:
      snprintf(buff, bufflen, "%d\n", PS3X8_GET_IO4(data));
      break;

      case 5:
      snprintf(buff, bufflen, "%d\n", PS3X8_GET_IO5(data));
      break;

      case 6:
      snprintf(buff, bufflen, "%d\n", PS3X8_GET_IO6(data));
      break;

      case 7:
      snprintf(buff, bufflen, "%d\n", PS3X8_GET_IO7(data));
      break;

      case 8:
      snprintf(buff, bufflen, "%d\n", PS3X8_GET_IO8(data));
      break;

      default:
			printlog(LOG_E, "ps3x8_get_digital_input() - unknown input: %d\n",  
            pd->digital_input_to_read);
         snprintf(buff, bufflen, "0\n");
   }


}
// }}}

// {{{ ps3x8_set_digital_output()
void ps3x8_set_digital_output(char *buff, size_t bufflen, void *udata)
{
   ps3x8_data_t *pd = (ps3x8_data_t*) udata;

   openlog("ps3x8", LOG_PID, LOG_USER);  

   int val = 0;
   if(strncmp(buff, "on", 2)==0)
      val = 1;
   if(strncmp(buff, "pulse", 2)==0)
      val = 2;

	if(val==1)
		create_output_value_file(pd->path, "on");
	else 
		create_output_value_file(pd->path, "off");

	printlog(LOG_N, "set output %d to %d (%s)\n", 
      pd->digital_output_to_write, val, buff);    

   semaphore_lock(w_data_fd);
   switch(pd->digital_output_to_write)
   {
      case 1:
			if(val==2)
			{
         	PS3X8_SET_IO1(1, *w_data);
         	ps3x8_write_digital_data(pd->device, *w_data);
				usleep(PULSE_INTERVAL);
         	PS3X8_SET_IO1(0, *w_data);
         	ps3x8_write_digital_data(pd->device, *w_data);
			}
			else
			{
         	PS3X8_SET_IO1(val, *w_data);
         	ps3x8_write_digital_data(pd->device, *w_data);
			}
         break;

      case 2:
			if(val==2)
			{
         	PS3X8_SET_IO2(1, *w_data);
         	ps3x8_write_digital_data(pd->device, *w_data);
				usleep(PULSE_INTERVAL);
         	PS3X8_SET_IO2(0, *w_data);
         	ps3x8_write_digital_data(pd->device, *w_data);
			}
			else
			{
         	PS3X8_SET_IO2(val, *w_data);
         	ps3x8_write_digital_data(pd->device, *w_data);
			}
         break;

      case 3:
			if(val==2)
			{
         	PS3X8_SET_IO3(1, *w_data);
         	ps3x8_write_digital_data(pd->device, *w_data);
				usleep(PULSE_INTERVAL);
         	PS3X8_SET_IO3(0, *w_data);
         	ps3x8_write_digital_data(pd->device, *w_data);
			}
			else
			{
         	PS3X8_SET_IO3(val, *w_data);
         	ps3x8_write_digital_data(pd->device, *w_data);
			}
         break;

      case 4:
			if(val==2)
			{
         	PS3X8_SET_IO4(1, *w_data);
         	ps3x8_write_digital_data(pd->device, *w_data);
				usleep(PULSE_INTERVAL);
         	PS3X8_SET_IO4(0, *w_data);
         	ps3x8_write_digital_data(pd->device, *w_data);
			}
			else
			{
         	PS3X8_SET_IO4(val, *w_data);
         	ps3x8_write_digital_data(pd->device, *w_data);
			}
         break;

      case 5:
			if(val==2)
			{
         	PS3X8_SET_IO5(1, *w_data);
         	ps3x8_write_digital_data(pd->device, *w_data);
				usleep(PULSE_INTERVAL);
         	PS3X8_SET_IO5(0, *w_data);
         	ps3x8_write_digital_data(pd->device, *w_data);
			}
			else
			{
         	PS3X8_SET_IO5(val, *w_data);
         	ps3x8_write_digital_data(pd->device, *w_data);
			}
         break;

      case 6:
			if(val==2)
			{
         	PS3X8_SET_IO6(1, *w_data);
         	ps3x8_write_digital_data(pd->device, *w_data);
				usleep(PULSE_INTERVAL);
         	PS3X8_SET_IO6(0, *w_data);
         	ps3x8_write_digital_data(pd->device, *w_data);
			}
			else
			{
         	PS3X8_SET_IO6(val, *w_data);
         	ps3x8_write_digital_data(pd->device, *w_data);
			}
         break;

      case 7:
			if(val==2)
			{
         	PS3X8_SET_IO7(1, *w_data);
         	ps3x8_write_digital_data(pd->device, *w_data);
				usleep(PULSE_INTERVAL);
         	PS3X8_SET_IO7(0, *w_data);
         	ps3x8_write_digital_data(pd->device, *w_data);
			}
			else
			{
         	PS3X8_SET_IO7(val, *w_data);
         	ps3x8_write_digital_data(pd->device, *w_data);
			}
         break;

      case 8:
			if(val==2)
			{
         	PS3X8_SET_IO8(1, *w_data);
         	ps3x8_write_digital_data(pd->device, *w_data);
				usleep(PULSE_INTERVAL);
         	PS3X8_SET_IO8(0, *w_data);
         	ps3x8_write_digital_data(pd->device, *w_data);
			}
			else
			{
         	PS3X8_SET_IO8(val, *w_data);
         	ps3x8_write_digital_data(pd->device, *w_data);
			}
         break;

      default:
			printlog(LOG_E, "ps3x8_set_digital_output() - unknown output: %d\n",  
            pd->digital_output_to_write);
   }
   semaphore_unlock(w_data_fd);

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
		printlog(LOG_E, "create_output_info_file() - fopen: %s : %s\n",  
         file, strerror(errno));
      return;
   }
   fprintf(f, "way=out\n");
   fprintf(f, "values=on,off,pulse\n");
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
		printlog(LOG_E, "create_input_info_file() - fopen: %s : %s\n",  
         file, strerror(errno));
      return;
   }
   fprintf(f, "way=in\n");
   fclose(f);
   chown(filename,1000,1000);
   chmod(filename, 0444);
}
// }}}



void usage(const char* prog)
{
   printf("%s -d [device] -p [dirname] -r [refresh interval] \n", prog);
   printf("%s -d [device] -t \n", prog);
   exit(0);
}

int main(int argc, char *argv[])
{
   int test_port = 0;
   int status;
   char c;

   char device[64] = "";
   char dirname[64] = "";
   char refresh[8] = "";
   char basedir[64] = "";
   char semname[64] = "";

   char di01[64] = "";
   char di02[64] = "";
   char di03[64] = "";
   char di04[64] = "";
   char di05[64] = "";
   char di06[64] = "";
   char di07[64] = "";
   char di08[64] = "";

   char do01[64] = "";
   char do02[64] = "";
   char do03[64] = "";
   char do04[64] = "";
   char do05[64] = "";
   char do06[64] = "";
   char do07[64] = "";
   char do08[64] = "";


   /* ps3x8 params, not used */
   while((c = getopt(argc, argv, "d:p:r:t"))>=0)
   {
      switch(c)
      {
         case 't':
            test_port = 1;
            break;

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

   if(c==-1 && test_port==0 && 
     (strlen(device)==0 ||strlen(dirname)==0||strlen(refresh)==0))
      usage(argv[0]);

   if(c==-1 && test_port==1 && strlen(device)==0)
      usage(argv[0]);


   if(test_port==1)
   {
      int dev = ps3x8_connect(device);
      if(dev<0)
      {
         puts("Device not found");  
         return 0;
      }

      puts("Micropik device found");
	   ps3x8_disconnect(dev);
      return 0;
   }



   /* Replace dangerous chars */
   // TODO: use regex
   str_replace(dirname, sizeof(dirname), ".", "_");
   str_replace(dirname, sizeof(dirname), "/", "_");

   /* Configure paths */
   snprintf(basedir, sizeof(basedir), DRIVER_PATH "/%s", dirname);
   snprintf(di01, sizeof(di01), DRIVER_PATH "/%s/di01", dirname);
   snprintf(di02, sizeof(di02), DRIVER_PATH "/%s/di02", dirname);
   snprintf(di03, sizeof(di03), DRIVER_PATH "/%s/di03", dirname);
   snprintf(di04, sizeof(di04), DRIVER_PATH "/%s/di04", dirname);
   snprintf(di05, sizeof(di05), DRIVER_PATH "/%s/di05", dirname);
   snprintf(di06, sizeof(di06), DRIVER_PATH "/%s/di06", dirname);
   snprintf(di07, sizeof(di07), DRIVER_PATH "/%s/di07", dirname);
   snprintf(di08, sizeof(di08), DRIVER_PATH "/%s/di08", dirname);

   snprintf(do01, sizeof(do01), DRIVER_PATH "/%s/do01", dirname);
   snprintf(do02, sizeof(do02), DRIVER_PATH "/%s/do02", dirname);
   snprintf(do03, sizeof(do03), DRIVER_PATH "/%s/do03", dirname);
   snprintf(do04, sizeof(do04), DRIVER_PATH "/%s/do04", dirname);
   snprintf(do05, sizeof(do05), DRIVER_PATH "/%s/do05", dirname);
   snprintf(do06, sizeof(do06), DRIVER_PATH "/%s/do06", dirname);
   snprintf(do07, sizeof(do07), DRIVER_PATH "/%s/do07", dirname);
   snprintf(do08, sizeof(do08), DRIVER_PATH "/%s/do08", dirname);

   snprintf(semname, sizeof(semname), "sem_%s", device);
   str_replace(semname, sizeof(semname), "/", "_");

   /* Remove old dirs */
   rmfr(basedir);

   /* Create working dir */
   mkdir(basedir, 0755);
   chown(basedir,1000,1000);


   int dev = ps3x8_connect(device);
	if(dev<0)
	{
		printf("ps3x8_connect() error, device: %s\n", device);
		return 0;
	}

   /* Shared data (w_data) */
   if((w_data_fd = open("/dev/zero", O_RDWR)) < 0)
   {
      perror("open");
      return 0;
   }

   if((w_data = mmap(0, sizeof(unsigned int), 
         PROT_READ | PROT_WRITE, MAP_SHARED, w_data_fd, 0)) == MAP_FAILED)
   {
      perror("mmap()");
      return 0;
   }

   *w_data = 0;


   ps3x8_data_t pd;
   pd.device = dev;


	/* eventlog */
	ps3x8_eventlog(&pd, atoi(refresh));

   /* Prepare digital inputs */
   pd.digital_input_to_read = 1;
   fifo_input(di01, ps3x8_get_digital_input, &pd);
   create_input_info_file(di01);

   pd.digital_input_to_read = 2;
   fifo_input(di02, ps3x8_get_digital_input, &pd);
   create_input_info_file(di02);

   pd.digital_input_to_read = 3;
   fifo_input(di03, ps3x8_get_digital_input, &pd);
   create_input_info_file(di03);

   pd.digital_input_to_read = 4;
   fifo_input(di04, ps3x8_get_digital_input, &pd);
   create_input_info_file(di04);

   pd.digital_input_to_read = 5;
   fifo_input(di05, ps3x8_get_digital_input, &pd);
   create_input_info_file(di05);

   pd.digital_input_to_read = 6;
   fifo_input(di06, ps3x8_get_digital_input, &pd);
   create_input_info_file(di06);

   pd.digital_input_to_read = 7;
   fifo_input(di07, ps3x8_get_digital_input, &pd);
   create_input_info_file(di07);

   pd.digital_input_to_read = 8;
   fifo_input(di08, ps3x8_get_digital_input, &pd);
   create_input_info_file(di08);



   /* Prepare digital outputs */
   pd.digital_output_to_write = 1;
	snprintf(pd.path, PATH_LEN, "%s", do01);
   fifo_output(do01, ps3x8_set_digital_output, &pd);
   create_output_info_file(do01);

   pd.digital_output_to_write = 2;
	snprintf(pd.path, PATH_LEN, "%s", do02);
   fifo_output(do02, ps3x8_set_digital_output, &pd);
   create_output_info_file(do02);

   pd.digital_output_to_write = 3;
	snprintf(pd.path, PATH_LEN, "%s", do03);
   fifo_output(do03, ps3x8_set_digital_output, &pd);
   create_output_info_file(do03);

   pd.digital_output_to_write = 4;
	snprintf(pd.path, PATH_LEN, "%s", do04);
   fifo_output(do04, ps3x8_set_digital_output, &pd);
   create_output_info_file(do04);

   pd.digital_output_to_write = 5;
	snprintf(pd.path, PATH_LEN, "%s", do05);
   fifo_output(do05, ps3x8_set_digital_output, &pd);
   create_output_info_file(do05);

   pd.digital_output_to_write = 6;
	snprintf(pd.path, PATH_LEN, "%s", do06);
   fifo_output(do06, ps3x8_set_digital_output, &pd);
   create_output_info_file(do06);

   pd.digital_output_to_write = 7;
	snprintf(pd.path, PATH_LEN, "%s", do07);
   fifo_output(do07, ps3x8_set_digital_output, &pd);
   create_output_info_file(do07);

   pd.digital_output_to_write = 8;
	snprintf(pd.path, PATH_LEN, "%s", do08);
   fifo_output(do08, ps3x8_set_digital_output, &pd);
   create_output_info_file(do08);


   wait(&status);
   printf("status: %d\n", status);

	/* unmap */
   munmap(0, sizeof(unsigned int));
   close(w_data_fd);

	/* close ps3x8 */
	ps3x8_disconnect(dev);


   return 0;
} 


