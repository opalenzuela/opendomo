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
 * date: December 2009
 *
 * CHANGES:
 * - Arduino firmware 0.4 support (Feb 2011)
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "semaphore.h"

#include <src/str.h>

#include "fifo.h"
#include "rmfr.h"
#include "readline.h"
#include "printlog.h"

#define PORT				23
#define BAUDRATE 			B9600
#define DRIVER_PATH 		"/var/opendomo/control"
#define CONF_PATH 		"/etc/opendomo/control"
#define NAME_LEN 			16
#define PATH_LEN 			64
#define VALUE_LEN			16
#define PORT_SIZE			128
#define TYPE_DI			1
#define TYPE_DO			2
#define TYPE_AI			3
#define TYPE_AO			4


char basedir[64] = "";
char confdir[64] = "";
char versn[16]="";
char bdate[16]="";
char build[16]="";

int debug=0;

// {{{ arduino_data_t
typedef struct arduino_data_t
{	
	int  fd;
   int  digital_input_to_read;
   int  analogic_input_to_read;
   int  digital_output_to_write;
	char path[PATH_LEN];
}
arduino_data_t;
// }}}

// {{{ arduino_io_t
typedef struct arduino_io_t
{
	char 	 port_name[PORT_SIZE][NAME_LEN];
	char 	 port_path[PORT_SIZE][PATH_LEN];
	char 	 port_value[PORT_SIZE][VALUE_LEN];
	size_t port_type[PORT_SIZE];
	pid_t  port_pid[PORT_SIZE];

} arduino_io_t;
// }}}

/* mmaped data */
int io_fd = 0;
arduino_io_t *io = NULL;

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

// {{{ create_output_info_file()
void create_output_info_file(const char *file)
{
   char filename[64];
   snprintf(filename, sizeof(filename), "%s", file);

	FILE *f;
	// Try to open the file for reading
	f = fopen(filename,"r");
	if (f != NULL) 
	{
		// If the file exists, abort
		printf("file exists: %s\n", filename);
		fclose(f);
		return;
    } 

   f = fopen(filename, "w+");
   if(!f)
   {
		printf("create_output_info_file() - fopen(): %s : %s",
         file, strerror(errno));
      return;
   }
   fprintf(f, "way=out\n");
   fprintf(f, "values=on,off\n");
   fprintf(f, "status=enabled\n");
   fclose(f);
   
   chmod(filename, 0644);
   chown(filename,1000,1000);
}
// }}}

// {{{ create_input_info_file()
void create_input_info_file(const char *file, int analog)
{
   char filename[64];
   snprintf(filename, sizeof(filename), "%s", file);

	FILE *f;
	// Try to open the file for reading
	f = fopen(filename,"r");
	if (f != NULL) 
	{
		printf("file exists: %s\n", filename);
		// If the file exists, abort
		fclose(f);
		return;
    } 

   f = fopen(filename, "w+");
   if(!f)
   {
		printf("create_input_info_file() - fopen(): %s : %s",
         file, strerror(errno));
      return;
   }
   fprintf(f, "way='in'\n");
   fprintf(f, "status='enabled'\n");
	//TODO: use "switch" for DI and "sensor" for AI
   fprintf(f, "type='switch'\n");

   fclose(f);
   chown(filename,1000,1000);
   chmod(filename, 0644);
}
// }}}

// {{{ arduino_find_id()
size_t arduino_find_id(const char *name)
{
	size_t i;
	for(i=0; i<PORT_SIZE; i++)
	{
		if(strcmp(io->port_name[i], name)==0)
			return i;
	}

	return -1;
}
// }}}

// {{{ arduino_serial_connect()
int arduino_serial_connect(const char *dev)
{
	int fd;
   struct termios oldtio, newtio;

   if( (fd = open(dev, O_RDWR | O_NOCTTY)) == -1)
   {
		printlog(LOG_E, "arduino_serial_connect() - open(): %s",
         strerror(errno));
      return -1; 
   }

   if(tcgetattr(fd, &oldtio) < 0)
   {
		printlog(LOG_E, "arduino_serial_connect() - tcgetattr(): %s", 
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
		printlog(LOG_E, "arduino_serial_connect() - tcflush(): %s", 
         strerror(errno));
      return -1;
   }

   if(tcsetattr(fd, TCSANOW, &newtio) < 0)
   {
		printlog(LOG_E, "arduino_serial_connect() - tcsetattr(): %s", 
         strerror(errno));
      return -1;
   }

	return fd;
}
// }}}

// {{{ arduino_network_connect()
int arduino_network_connect(const char *host)
{
	struct hostent *host_name;
	if((host_name = gethostbyname(host))==0) 
	{
		printlog(LOG_E, "arduino_net_connect() - gethostbyname(): %s",
         strerror(errno));
      return -1; 
	}
	
	struct sockaddr_in pin; 
	memset(&pin, 0, sizeof(pin));
	pin.sin_family =  AF_INET;
	pin.sin_addr.s_addr = htonl(INADDR_ANY);
	pin.sin_addr.s_addr = ((struct in_addr *)(host_name->h_addr))->s_addr;
	pin.sin_port = htons(PORT);

	int sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1) 
	{ 
		printlog(LOG_E, "arduino_net_connect() - socket(): %s",
         strerror(errno));
      return -1; 
	}

	if(connect(sd, (void *)&pin, sizeof(pin))==-1) 
	{
		printlog(LOG_E, "arduino_net_connect() - connect(): %s",
         strerror(errno));
      return -1; 
	}

	return sd;
}
// }}}

// {{{ arduino_connect()
int arduino_connect(const char *dev)
{
	if(strncmp(dev, "net:", 4)==0)
		return arduino_network_connect(dev+4);

	return arduino_serial_connect(dev);
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
	if(debug)
		printf("arduino_send_cmd(): %s\n", cmd);

	if(write(fd, cmd, strlen(cmd)) <=0)
	{
		printlog(LOG_E, "arduino_connect() - write(): %s",
         strerror(errno));
		return;
	}
}
// }}}

// {{{ arduino_get_digital_input()
void arduino_get_digital_input(char *buff, size_t bufflen, void *udata)
{
   arduino_data_t *dd = (arduino_data_t*) udata;

	if(strncmp(io->port_value[dd->digital_input_to_read], "on", 2)==0 ||
		strncmp(io->port_value[dd->digital_input_to_read], "ON", 2)==0 ||
		strncmp(io->port_value[dd->digital_input_to_read], "On", 2)==0 ||
		strncmp(io->port_value[dd->digital_input_to_read], "1", 1)==0)
		strcpy(buff, "on"); 

	else if(strncmp(io->port_value[dd->digital_input_to_read], "off", 3)==0 ||
		strncmp(io->port_value[dd->digital_input_to_read], "OFF", 3)==0 ||
		strncmp(io->port_value[dd->digital_input_to_read], "Off", 3)==0 ||
		strncmp(io->port_value[dd->digital_input_to_read], "0", 1)==0)
		strcpy(buff, "off"); 

	else
		strcpy(buff, "off"); 

   snprintf(buff, bufflen, "%s\n", buff);
}
// }}}

// {{{ arduino_get_analogic_input()
void arduino_get_analogic_input(char *buff, size_t bufflen, void *udata)
{
   arduino_data_t *dd = (arduino_data_t*) udata;
   snprintf(buff, bufflen, "%s\n", io->port_value[dd->analogic_input_to_read]);
}
// }}}

// {{{ arduino_set_digital_output()
void arduino_set_digital_output(char *buff, size_t bufflen, void *udata)
{
	char cmd[64];
	char value[64];
   arduino_data_t *dd = (arduino_data_t*) udata;

	printlog(LOG_N, "set output [%d] to [%s]\n", 
		dd->digital_output_to_write, buff);


	int idx = dd->digital_output_to_write;
	
	if(strncmp(buff, "on", 2)==0 ||
		strncmp(buff, "ON", 2)==0 ||
		strncmp(buff, "On", 2)==0 ||
		strncmp(buff, "1", 1)==0)
		strcpy(value, "on"); 

	else if(strncmp(buff, "off", 3)==0 ||
		strncmp(buff, "OFF", 3)==0 ||
		strncmp(buff, "Off", 3)==0 ||
		strncmp(buff, "0", 1)==0)
		strcpy(value, "off"); 

	else
		strcpy(value, "off"); 
	
	create_output_value_file(dd->path, value);

   semaphore_lock(io_fd);
	snprintf(io->port_value[idx], VALUE_LEN, "%s", value);
	snprintf(cmd, sizeof(cmd), "set %s %s\r", 
		io->port_name[idx], io->port_value[idx]);
	arduino_send_cmd(dd->fd, cmd);
   semaphore_unlock(io_fd);
	
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
				int id = 0;
				int port_type = 0;
				char type = 0;
				char name[16];
				char board[16];
				char v1[16];
				char v2[16];
				char v3[16];
				char v4[16];
				char v5[16];

				
				if(strncmp(line, "N:cmd ls", 8)==0)
				{
					id_count=1;
					continue;
				}

				memset(name, 0, sizeof(name));
				memset(v1, 0, sizeof(v1));
				memset(v2, 0, sizeof(v2));
				memset(v3, 0, sizeof(v3));
				memset(v4, 0, sizeof(v3));
				memset(v5, 0, sizeof(v3));

				sscanf(line, 
				"%c:%15[^.].%15[^ ] %15[^ ] %15[^ ] %15[^ ] %15[^ ] %15[^ ]", 
				&type, board, name, v1, v2, v3, v4, v5);


				if(debug)
					printf("%s\n", line);

				//if(debug)
				printf("params: %c|%s|%s|%s|%s|%s|%s\%s\n", 
						type, board, name, v1, v2, v3, v4, v5);


				switch(type)
				{
					case 'I':
					// {{{

					/* read id*/
					id = id_count;
					id_count++;

					if(id_count>PORT_SIZE)
					{
						printf("FATAL ERROR id_count > PORT_SIZE\n");
						exit(0);
					}

					/* read port type  */
					if			(strncmp(v1, "DO", 2)==0)
						port_type = TYPE_DO;
					else if	(strncmp(v1, "DI", 2)==0)
						port_type = TYPE_DI;
					else if	(strncmp(v1, "AO", 2)==0)
						port_type = TYPE_AO;
					else if	(strncmp(v1, "AI", 2)==0)
						port_type= TYPE_AI;
					else if	(strncasecmp(v1, "X----", 5)==0)
						continue;
					else if	(strncasecmp(v1, "V----", 5)==0)
						continue;
					else if	(strncmp(name, "versn", 5)==0)
					{
						snprintf(versn, sizeof(versn), "%s", v1);
						printf("versn: %s\n", versn);
						continue;
					}
					else if	(strncmp(name, "bdate", 5)==0)
					{
						snprintf(bdate, sizeof(bdate), "%s %s %s", v1, v2, v3);
						printf("bdate: %s\n", bdate);
						continue;
					}
					else if	(strncmp(name, "build", 5)==0)
					{
						snprintf(build, sizeof(build), "%s", v1);
						printf("build: %s\n", build);
						continue;
					}
					else
					{
						printf("unknown type: %s\n", v1);
						continue;
					}
	
					/* I/O */
					arduino_data_t ad;
					ad.fd = fd;
					char path[64];
					char pathi[64];
					int create_new_pipe = 0;

					/* actual path*/
					snprintf(path, sizeof(path), "%s/%s", basedir,io->port_name[id]);


					if(strlen(io->port_name[id])==0)
					{
						semaphore_lock(io_fd);
						snprintf(io->port_name[id], NAME_LEN, "%s", name);
   					semaphore_unlock(io_fd);
					}

					/* the name or the type has changed */
					if(strcmp(name, io->port_name[id])!=0 ||
					   io->port_type[id] != port_type )
					{
						create_new_pipe = 1;
						if(io->port_pid[id]>0)
						{
							printf("killing: %d\n", io->port_pid[id]);
							if((kill(io->port_pid[id], SIGKILL)!=0))
							{
								perror("kill()");
								continue;
							}
							
						}
						unlink(path);
					}

					/* set new path*/
					snprintf(path, sizeof(path), "%s/%s", basedir, name);
					snprintf(ad.path, PATH_LEN, "%s", path);

					snprintf(pathi, sizeof(pathi),
						"%s/%s.info", confdir, io->port_name[id]);

					/* set new values */
					semaphore_lock(io_fd);
					io->port_type[id] = port_type;
					snprintf(io->port_value[id], VALUE_LEN, "%s", v2);
					snprintf(io->port_name[id], NAME_LEN, "%s", name);
   				semaphore_unlock(io_fd);

					/* create pipe if needed */
					if(create_new_pipe)
					switch(io->port_type[id])
					{
						case TYPE_DI:
						ad.digital_input_to_read = id;
   					semaphore_lock(io_fd);
						io->port_pid[id] = 
							fifo_input(path, arduino_get_digital_input, &ad);
   					semaphore_unlock(io_fd);
						create_input_info_file(pathi, 0);
						break;

						case TYPE_DO:
						ad.digital_output_to_write = id;
   					semaphore_lock(io_fd);
						io->port_pid[id] = 
   						fifo_output(path, arduino_set_digital_output, &ad);
   					semaphore_unlock(io_fd);
   					create_output_info_file(pathi);
						create_output_value_file(path, v2);
						break;

						case TYPE_AI:
						ad.analogic_input_to_read = id;
   					semaphore_lock(io_fd);
						io->port_pid[id] = 
							fifo_input(path, arduino_get_analogic_input, &ad);
   					semaphore_unlock(io_fd);
						create_input_info_file(pathi, 1);
						break;

						default:
						//printlog(LOG_E, "arduino_process_log() - "
						//	"unknown type %d\n", io->port_type[id]); 
						break;
					}

					// }}}
					break; 

					case 'N':
					// {{{
					printlog(LOG_N, "%s.%s %s %s %s %s %s", 
						board, name, v1, v2, v3, v4, v5);

				
					if(strcmp(name, "board")==0)
					{
						/* load */
						if	(strncmp(v1, "cfloa", 5)==0)
						{
							puts("load");
							arduino_send_cmd(fd, "lst\r");
						}

						/* default */
						if	(strncmp(v1, "cfdef", 5)==0)
						{	
							puts("default conf");
							arduino_send_cmd(fd, "lst\r");
						}

						continue;
					}

					id = arduino_find_id(name);

					if(id==-1)
					{
						printf("arduino_process_log() - %s not found\n", name); 
						continue;
					}
	

					/* assign new value */
					if			(strcmp(v1, "value")==0)
					{
						printf("set %s to %s\n", name, v2);
   					semaphore_lock(io_fd);
						snprintf(io->port_value[id], VALUE_LEN, "%s", v2);
   					semaphore_unlock(io_fd);
					}

				   // }}}
					break;

					case 'D':
					break;

					case 'W':
					printlog(LOG_W, "%s.%s %s %s %s %s %s", 
						board, name, v1, v2, v3, v4, v5);
					break;

					case 'A':
					printlog(LOG_A, "%s.%s %s %s %s %s %s", 
						board, name, v1, v2, v3, v4, v5);
					break;

					case 'E':
						if(strcmp(v1, "error")==0)
						{
							int eval = atoi(v2);

							switch(eval)
							{
								case 1:
								printlog(LOG_E, "Command not found");
								break;

								default:
								printf("Unknown error: %s\n", v2);

							}
						}
					break;
				}
			}
		}

		exit(0);
	}

	/* Parent */
}
// }}}

// {{{ arduino_refresh_loop()
void arduino_refresh_loop(pid_t arduino_fd)
{
	time_t t=0;
	int p;

	/* Child process */
	if((p=fork())==0)
	{
		sleep(5);
		arduino_send_cmd(arduino_fd, "ver\r");
		arduino_send_cmd(arduino_fd, "lst\r");

		for(;;)
		{
			if(time(NULL) > t+30)
			{
				arduino_send_cmd(arduino_fd, "lst\r");
				t=time(NULL);
			}
			usleep(500000);
		}

		exit(0);
	}

	/* Parent */
}
 // }}}

// {{{ arduino_user_config()
void arduino_user_config(const char* path, pid_t arduino_fd)
{
   int fd = 0;
   char buff[64];
   char cmd[64];

	mkfifo(path, 0722);
	chmod(path, 0722);
	fd = open(path, O_RDONLY);
	if(fd<0)
	{
		printlog(LOG_E, "arduino_user_config() - open() : "
			"fifo_output  %s : %s", path, strerror(errno));
		return;
	}

	while(1)
	{
		int n = readline(buff, sizeof(buff), fd);
		if(n<=0)
			continue;
		
		buff[n+1] = 0;

		/* Process comments */
		if(strstr(buff, "#"))
			continue;

		snprintf(cmd, sizeof(cmd), "%s\r", buff);
		printf("command send: %s\n", buff);
		arduino_send_cmd(arduino_fd, cmd);
	}

	close(fd);
	unlink(path);
}
// }}}

// {{{ usage()
void usage(const char* prog)
{
   printf("%s -d [device] -p [dirname] -r [refresh interval] \n", prog);
   exit(0);
}
// }}}

int main(int argc, char *argv[])
{
   int status;
   char c;

   char device[64] = "";
   char dirname[64] = "";
   char refresh[8] = "";

   /* params */
   while((c = getopt(argc, argv, "d:p:r:D"))>=0)
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

         case 'D':
				debug=1;
            break;
      }
   }

   if(c==-1 && (strlen(device)==0 ||strlen(dirname)==0||strlen(refresh)==0))
      usage(argv[0]);

   /* Replace dangerous chars */
   // TODO: use regex
   str_replace(dirname, sizeof(dirname), ".", "_");
   str_replace(dirname, sizeof(dirname), "/", "_");

   /* Configure paths */
   snprintf(basedir, sizeof(basedir), DRIVER_PATH "/%s", dirname);
   snprintf(confdir, sizeof(confdir), CONF_PATH "/%s", dirname);

   /* Remove old dirs */
   //rmfr(basedir);

   /* Create working dir */
   mkdir(basedir, 0755);
   chown(basedir,1000,1000);

	/* Connect to arduino */
	int arduino_fd = arduino_connect(device);
	if(arduino_fd<0)
	{
		perror("arduino_connect(): cannot connect to Arduino device");
		exit(EXIT_FAILURE);
	}
	
   /* Shared data (io) */
   if((io_fd = open("/dev/zero", O_RDWR)) < 0)
   {
      perror("open");
		exit(EXIT_FAILURE);
   }

   if((io = mmap(0, sizeof(arduino_io_t), 
         PROT_READ | PROT_WRITE, MAP_SHARED, io_fd, 0)) == MAP_FAILED)
   {
      perror("mmap()");
		exit(EXIT_FAILURE);
   }

   memset(io, 0, sizeof(arduino_io_t));

	/* process arduino's output */
	arduino_process_log(arduino_fd);

	/* refresh */
	arduino_refresh_loop(arduino_fd);

	/* user commands */
	char path[64];
	snprintf(path, sizeof(path), "%s/config", basedir);
	arduino_user_config(path, arduino_fd);

	/* wait childs */
   wait(&status);
   printf("status: %d\n", status);

	/* close Arduino conn */
	arduino_close(arduino_fd);

	/* unmap and close */
   munmap(0, sizeof(arduino_io_t));
   close(io_fd);

   return 0;
} 


