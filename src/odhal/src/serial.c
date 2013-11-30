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
 * date: Mar 2011
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
#include <sys/mman.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

#include "readline.h"


#define BAUDRATE 			B9600



// {{{ arduino_connect()
int arduino_connect(const char *dev)
{
	int fd;
   struct termios oldtio, newtio;

   if( (fd = open(dev, O_RDWR | O_NOCTTY)) == -1)
   {
		printf("arduino_connect() - open(): %s",
         strerror(errno));
      return -1; 
   }

   if(tcgetattr(fd, &oldtio) < 0)
   {
		printf("arduino_connect() - tcgetattr(): %s", 
         strerror(errno));
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
		printf("arduino_connect() - tcflush(): %s", 
         strerror(errno));
      return -1;
   }

   if(tcsetattr(fd, TCSANOW, &newtio) < 0)
   {
		printf("arduino_connect() - tcsetattr(): %s", 
         strerror(errno));
      return -1;
   }

	return fd;
}
// }}}

// {{{ arduino_close()
void arduino_close(int fd)
{
	close(fd);
}
// }}}

// {{{ arduino_send_cmd()
void arduino_send_cmd(int fd, const char *cmd)
{
	if(write(fd, cmd, strlen(cmd)) <=0)
	{
		printf("arduino_connect() - write(): %s",
         strerror(errno));
		return;
	}
}
// }}}

// {{{ arduino_process_log()
void arduino_process_log(int fd)
{
	char line[64];	
	int p;

	/* Child process */
	if((p=fork())==0)
	{
		for(;;)
		{
			int id_count = 1;
			int n;
		   while((n=readline(line, sizeof(line), fd)) > 0)
			{
				printf("%s\n", line);
			}
		}

		exit(0);
	}

	/* Parent */
}
// }}}

int main(int argc, char *argv[])
{
	if(argc!=2)
	{
		printf("Usage: %s <device>\n", argv[0]);
		exit(0);
	}

	/* Connect to arduino */
	int arduino_fd = arduino_connect(argv[1]);
	if(arduino_fd<0)
	{
		perror("arduino_connect(): cannot connect to Arduino device");
		exit(EXIT_FAILURE);
	}
	
	arduino_process_log(arduino_fd);
	
	char buffer[128];
	while(1)
	{
		printf("cmd> ");
		fgets(buffer, sizeof(buffer), stdin);

		if(strlen(buffer)<2)
			continue;

		if(strncasecmp(buffer, "quit", 4)==0)
			exit(0);

		if(strlen(buffer)<sizeof(buffer))
		{
			buffer[strlen(buffer)-1]='\r';
			arduino_send_cmd(arduino_fd, buffer);
		}
	}


	arduino_close(arduino_fd);

   return 0;
} 


