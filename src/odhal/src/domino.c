/*****************************************************************************
 *  This file is part of the OpenDomo project.
 *  Copyright(C) 2013 OpenDomo Services SL
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
#include <sys/mman.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ether.h>




#include <src/str.h>


#include "fifo.h"
#include "rmfr.h"
#include "printlog.h"

#define TCP_PORT 1729
#define UDP_PORT 1729
#define DRIVER_PATH 	"/var/opendomo/control"
#define CONF_PATH 	"/etc/opendomo/control"
#define CONF_FILE		"/var/opendomo/domino_devices.conf"
#define LOG_PATH		"/var/opendomo/log/"
#define PATH_LEN 64
#define DEVICEDOWN_STR   "DEVICEDOWN"
#define DEVICEDOWN_TIME 30


#define strNcpy(a,b,c) strncpy(a,b,c); a[c-1]=0;


typedef struct domino_data_t
{
	char ip[16];
	char pname[6];
	char path[PATH_LEN];
}
domino_data_t;

void notify_port_change(char *pname, char *pvalue){
	char cmd[512];
	char script[] = "/usr/local/opendomo/services/syscript/eh_outputstates.sh";

	snprintf(cmd, sizeof(cmd), "%s portchange odhal '%s %s'", 
		script, pname, pvalue);
	system(cmd);
}



/// {{{ is_value_correct()
int is_value_correct(char *value)
{
	int i;
	int cnt=0;
	if (strncmp(value,"ON",2)==0 || strncmp(value,"OFF",3)==0) return 1;

	for(i=0; i<strlen(value); i++)
	{
		if ((value[i]>=48 && value[i]<=57)||value[i]=='.'||value[i]=='-'||value[i]=='+') cnt++;
	}
//		if(isprint(value[i])||isspace(value[i]))
//			cnt++;

	if(cnt == strlen(value) && cnt > 0)
	{
		//printf("Value accepted: '%s'\n", value);
		return 1;
	}
	
	//printf("Value rejected: '%s'\n", value);
	return 0;
}
// }}}

// {{{ save_stats()
void save_stats(const char *uid, const char *name, const char *value)
{
	char filename[64];
	time_t t = time(NULL);
	struct tm *stm = localtime(&t);
	snprintf(filename, sizeof(filename), 
		LOG_PATH "/%s-%s.h%02d", uid, name, stm->tm_hour);

	FILE *f = fopen(filename, "a+");
	if(!f)
	{
		printlog(LOG_E, "save_stats() : fopen() : %s : %s\n", 
		filename, strerror(errno));
		return;
	}

	fprintf(f, "%ld %s\n", t, value);

	fclose(f);
}
// }}}

// {{{ create_input_info_file()
void create_input_info_file(const char *file, int isdigital)
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
	
	fprintf(f, "way='in'\n");
	fprintf(f, "tag='none'\n");   
	fprintf(f, "zone='none'\n");   
	fprintf(f, "status='enabled'\n");
	if(isdigital) 
	{
		fprintf(f, "type='switch'\n");
		fprintf(f, "values='on,off'\n");
	}
	else
	{
		fprintf(f, "type='sensor'\n");
		fprintf(f, "values='00,10,20,30,40,50,60,70,80,90,100'\n");
	}
	fclose(f);
	chown(filename,1000,1000);
	chmod(filename, 0644);
}
// }}}

// {{{ create_output_info_file()
void create_output_info_file(const char *file, int isdigital)
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
	fprintf(f, "way='out'\n");
	fprintf(f, "tag='none'\n");   
	fprintf(f, "zone='none'\n");    
	if (isdigital)  
	{
		fprintf(f, "values='on,off'\n");
	}
	else
	{
		fprintf(f, "values='00,10,20,30,40,50,60,70,80,90,100'\n");	
	}
	fprintf(f, "status='enabled'\n");
	fclose(f);

	chmod(filename, 0644);
	chown(filename,1000,1000);
}
// }}}

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

	chmod(filename, 0644);
	chown(filename,1000,1000);
}
// }}}

// {{{ create_input_value_file()
void create_input_value_file(const char *path, const char *value)
{
	char filename[64];

	// Creation of the ".value" file
	snprintf(filename, sizeof(filename), "%s.value", path);

	FILE *f = fopen(filename, "w");
	fprintf(f, "%s\n", value);
	fclose(f);

	chmod(filename, 0644);
	chown(filename,1000,1000);
	
	// Creation of the readonly file
	snprintf(filename, sizeof(filename), "%s", path);

	FILE *f2 = fopen(filename, "w");
	fprintf(f2, "%s\n", value);
	fclose(f2);

	chmod(filename, 0444);
	chown(filename,1000,1000);	
}
// }}}

// {{{ domino_set_digital_output()
void domino_set_digital_output(char *buff, size_t bufflen, void *udata)
{
	domino_data_t *dd = (domino_data_t*) udata;
	printlog(LOG_N, "set output [%s] to [%s]\n", dd->pname, buff);

	char buf[64];
	char cmd[64];
	snprintf(cmd, sizeof(cmd), "set %s %s\n", dd->pname, buff);
	if(domino_exec_command(dd->ip, cmd, buf, sizeof(buf))==0)
	{
		create_output_value_file(dd->path, buff);
	}
	else
	{
		printlog(LOG_E, "domino_set_digital_output() - "
			"domino_exec_command(): %s", strerror(errno));
	}
}
// }}}

// {{{ domino_set_analog_output()
void domino_set_analog_output(char *buff, size_t bufflen, void *udata)
{
	domino_data_t *dd = (domino_data_t*) udata;
	printlog(LOG_N, "set analog output [%s] to [%s]\n", dd->pname, buff);

	char buf[64];
	char cmd[64];
	//FIXME: Decimals added manually
	snprintf(cmd, sizeof(cmd), "set %s %s0000\n", dd->pname, buff);
	if(domino_exec_command(dd->ip, cmd, buf, sizeof(buf))==0)
	{
		create_output_value_file(dd->path, buff);
	}
	else
	{
		printlog(LOG_E, "domino_set_analog_output() - "
			"domino_exec_command(): %s", strerror(errno));
	}
}
// }}}

// {{{ domino_exec_command()
int domino_exec_command(char *host, char *cmd, char *result, size_t result_len)
{
	struct hostent *host_name;
	if((host_name = gethostbyname(host))==0) 
	{
		printlog(LOG_E, "domino_exec_command() - gethostbyname(): %s, %s",
		host, strerror(errno));
		return -1; 
	}
	
	struct sockaddr_in pin; 
	memset(&pin, 0, sizeof(pin));
	pin.sin_family =  AF_INET;
	pin.sin_addr.s_addr = htonl(INADDR_ANY);
	pin.sin_addr.s_addr = ((struct in_addr *)(host_name->h_addr))->s_addr;
	pin.sin_port = htons(TCP_PORT);

	int sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1) 
	{ 
		printlog(LOG_E, "domino_exec_command() - socket(): %s", strerror(errno));
		return -1; 
	}

	if(connect(sd, (void *)&pin, sizeof(pin))==-1) 
	{
		printlog(LOG_E, "domino_exec_command() - connect(): %s",
		strerror(errno));
		close(sd);
		return -1; 
	}

	
	send(sd, cmd, strlen(cmd), 0);
	int s = recv(sd, result, result_len, 0);
	if(s>0 && s<result_len)
		result[s]=0;
	else
		result[0]=0;

	close(sd);
	return 0;
}
// }}}

// {{{ domino_check_device_list()
int domino_check_device_list()
{
	int ndevs=0;

	FILE *f = fopen(CONF_FILE, "r+");
	if(!f)
	{
		return -1;
	}

	while(!feof(f))
	{
		char uid_f[13]; 
		char ip_f[16]; 
		char bname_f[6]; 
		char timestamp_f[11];

		// read line
		char line[64];
		if(!fgets(line, sizeof(line), f))
			break;
		strNcpy(uid_f, line, sizeof(uid_f)); 
		strNcpy(ip_f, line+13, sizeof(ip_f));
		strNcpy(bname_f, line+29, sizeof(bname_f));
		strNcpy(timestamp_f, line+35, sizeof(timestamp_f));

		if(strlen(bname_f)==0 || 
			strlen(uid_f)==0 || 
			strlen(ip_f)==0 || 
			strlen(timestamp_f)==0)
			continue;

		if(strcmp(timestamp_f, DEVICEDOWN_STR)!=0)
		{
			int df = (int)time(NULL)-atoi(timestamp_f);
			if(df>DEVICEDOWN_TIME)
			{
				fseek(f, -46, SEEK_CUR);
				fprintf(f, "%s:%15s:%s:%s\n", uid_f, ip_f, bname_f, 
						DEVICEDOWN_STR);
			}
			else
				ndevs++;
		}
		else
		{	// remove old directories
			char base[64];
			snprintf(base, sizeof(base), DRIVER_PATH "/ODC%s", uid_f);
			if(strlen(base)==34) // simple verification
				rmfr(base);
		}
	}

	return ndevs;
}
// }}}

// {{{ domino_update_list()
/*
	Configuration file:
	uid(12)     :ip(15)         :id(5):timestamp(14)
	ABC56DE9465D:192.168.001.177:odctl:0000000000
	123456789012345678901234567890123456789012345
	         1         2         3         4
*/
void domino_update_list(
		const char *proto, 	
		const char *bname,
		const char *uid, 
		const char *ip)
{
	if(strcmp(proto, "ODC01")!=0)
	{
		printlog(LOG_E, "domino_update_list() - incorrect protocol version: %s",
         	proto);
		return;
	}
	int i=0;
	for (i=0;i<5;i++) {
		if (!isalnum(bname[i])){
			printlog(LOG_E, "domino_update_list() - invalid devname [%d]", 
				bname[i]);			
     		return;
		}
	}

	FILE *f = fopen(CONF_FILE, "r+");
	if(!f)
		f = fopen(CONF_FILE, "w+");

	if(!f)
	{
		printlog(LOG_E, "domino_update_list() - fopen(): %s", strerror(errno));
		return;
	}

	while(!feof(f))
	{
		char uid_f[13]; 
		char ip_f[16]; 
		char bname_f[6]; 
		char timestamp_f[11];

		// read line
		char line[64];
		fgets(line, sizeof(line), f);
		strNcpy(uid_f, line, sizeof(uid_f)); 
		strNcpy(ip_f, line+13, sizeof(ip_f));
		strNcpy(bname_f, line+29, sizeof(bname_f));
		strNcpy(timestamp_f, line+35, sizeof(timestamp_f));

		if(strlen(bname_f)==0 || 
			strlen(uid_f)==0 || 
			strlen(ip_f)==0 || 
			strlen(timestamp_f)==0)
			continue;

		/* update timestamp */
		if(strcmp(uid_f, uid)==0)
		{
			puts("updated");
			fseek(f, -46, SEEK_CUR);
			fprintf(f, "%s:%15s:%s:%d\n", uid, ip, bname, (int)time(NULL));
			fclose(f); 
			return;
		}
	}

	if(f)
	{
		puts("added");
		fprintf(f, "%s:%15s:%s:%d\n", uid, ip, bname, (int)time(NULL));
		fclose(f);
		char cmd[1024];
		snprintf(cmd, sizeof(cmd), 
			"/usr/local/bin/bgshell \"/usr/local/bin/odhal_init.sh %s %s >>/var/opendomo/log/odhaldetection.log\"", ip, uid);
		printlog(LOG_E, "executing: %s", cmd);
		system(cmd);
	}

	//chown(CONF_FILE, 1000, 0); // TODO uncoment in ODNetwork version
}
// }}}

// {{{ domino_update_files()
void domino_update_files(
		const char *proto, 	
		const char *uid, 	
		const char *type, 	
		const char *name, 	
		const char *value)
{
	char drv_path[64];
	char cnf_path[64];
	char base[64];
	char dbg[1024];


	printlog(LOG_D, dbg,"Updating value %s '%s'\n", name, value);

	snprintf(base, sizeof(base), DRIVER_PATH "/ODC%s", uid);
	snprintf(drv_path, sizeof(drv_path), DRIVER_PATH "/ODC%s/%s", uid, name);
	mkdir(base, 0755);


	snprintf(base, sizeof(base), CONF_PATH "/ODC%s", uid);
	snprintf(cnf_path, sizeof(cnf_path), CONF_PATH "/ODC%s/%s", uid, name);
	mkdir(base, 0755);

	printlog(LOG_D, dbg, "update file: %s, %s, %s, %s, %s\n", 
		proto, uid, type, name, value);

	if(strcmp(type, "DO")==0)
	{
		create_output_info_file(cnf_path, 1);
		create_output_value_file(drv_path, value);
	}

	if(strcmp(type, "AO")==0)
	{
		create_output_info_file(cnf_path, 0);
		create_output_value_file(drv_path, value);
	}

	if(strcmp(type, "DI")==0)
	{
		create_input_info_file(cnf_path, 1);
		create_input_value_file(drv_path, value);
	}
 
	if(strcmp(type, "AI")==0)
	{
		create_input_info_file(cnf_path, 0);
		create_input_value_file(drv_path, value);
		save_stats(uid, name, value);
	}

	if(strcmp(type, "AV")==0 )
	{
		create_output_info_file(cnf_path, 0);
		create_output_value_file(drv_path, value);
		save_stats(uid, name, value);
	}	

	notify_port_change(name,value);
	fflush(stdout);
}
// }}}

// {{{ domino_process_incoming_messages()
void domino_process_incoming_messages()
{
	struct sockaddr_in si_me, si_other;
	int s;
	size_t slen=sizeof(si_other);
	char buf[256];
	char dbg[1024];

	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
	{
		printlog(LOG_E, "domino_process_incoming_messages() - socket(): %s",
		strerror(errno));
		return;
	}

	memset((char *) &si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(UDP_PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(s, (const struct sockaddr *)&si_me, sizeof(si_me))==-1)
	{
		printlog(LOG_E, "domino_process_incoming_messages() - bind(): %s",
		strerror(errno));
		return;
	}

	int bytes;
	while(1)
	{
		if ((bytes=recvfrom(s, buf, sizeof(buf), 0, 
			(struct sockaddr *)&si_other, &slen))==-1)
		{
			printlog(LOG_E, "domino_process_incoming_messages() - recvfrom(): %s",
         	strerror(errno));
			return;
		}

		buf[bytes]=0;

		snprintf(dbg,sizeof(dbg),"package: %s\n", buf);
		printlog(LOG_D, dbg);

		/* is alive packet */
		if(strstr(buf, "isalive"))
		{
			char proto[6];
			char bname[6];
			char uid[13];
			sscanf(buf, "%5s:%12s:%5s isalive", proto, uid, bname);
			domino_update_list(proto, bname, uid, inet_ntoa(si_other.sin_addr));
		}
		
		/* port info packet */
		else
		{
			char proto[6];
			char uid[13];
			char type[3];
			char name[6];
			char value[16];
			
			char *pch = strtok (buf, " \n\r");
			while (pch != NULL)
			{
				if(strncmp("ODC", pch, 3)==0)
				{
					strNcpy(proto, pch, sizeof(proto));
					strNcpy(uid, pch+sizeof(proto), sizeof(uid));
				}
				else
				{
					strNcpy(type, pch, sizeof(type));
					strNcpy(name, pch+sizeof(type), sizeof(name));
					strNcpy(value, pch+sizeof(type)+sizeof(name), sizeof(value));

					if(1==is_value_correct(value)) 
					{
						printlog(LOG_D, dbg,"Correct value %s",value);
						domino_update_files(proto, uid, type, name, value);
					}	
					else 
					{
						printlog(LOG_D, dbg,"Invalid value %s",value);
					}	
				}
				pch = strtok (NULL, " \n\r");
			}
		}	
	}
	fflush(stdout);
}
// }}}

// {{{ domino_manage_output_files()
void domino_manage_output_files()
{
	// TODO: Falta borrar los puertos que ya no existen

	FILE *f = fopen(CONF_FILE, "r");

	while(!feof(f))
	{
		char uid_f[13];
		char ip_f[16];
		char bname_f[6];
		char timestamp_f[11];

		// read line
		char line[64];
		fgets(line, sizeof(line), f);
		strNcpy(uid_f, line, sizeof(uid_f));
		strNcpy(ip_f, line+13, sizeof(ip_f));
		strNcpy(bname_f, line+29, sizeof(bname_f));
		strNcpy(timestamp_f, line+35, sizeof(timestamp_f));
		str_trim(ip_f);

		printlog(LOG_D,"Listing ports on %s\n",ip_f);

		char base[64];
		snprintf(base, sizeof(base), DRIVER_PATH "/ODC%s", uid_f);
		mkdir(base, 0755);

		snprintf(base, sizeof(base), CONF_PATH "/ODC%s", uid_f);
		mkdir(base, 0755);

		char buf[1024];
		// First LST
		if(domino_exec_command(ip_f, "lst   \n", buf, sizeof(buf))!=0)
		{
			printlog(LOG_E, "domino_manage_output_files() - "
				"domino_exec_command(): %s", strerror(errno));
			continue;
		} else {
			domino_manage_output_files_buffer(buf, uid_f, ip_f);
		}

		strNcpy(buf,"",sizeof(buf));
	
		// Second LST (Version 1.5.1 and above)
		if(domino_exec_command(ip_f, "lst 24-48 \n", buf, sizeof(buf))!=0)
		{
			printlog(LOG_E, "domino_manage_output_files() - "
				"domino_exec_command(): %s", strerror(errno));
			continue;
		} else {
			domino_manage_output_files_buffer(buf, uid_f, ip_f);
		}
		

		//printf("Output: %s\n", buf);
	}

	fclose(f);
}
// }}}

// {{{ domino_manage_output_files_buffer()
void domino_manage_output_files_buffer(char* buf, char * uid_f, char * ip_f) 
{
	char *pch = strtok (buf, " \n\r");

	while (pch != NULL)
	{
		char type[3];
		char name[6];
		char value[16];
		strNcpy(type, pch, sizeof(type));
		strNcpy(name, pch+sizeof(type), sizeof(name));
		strNcpy(value, pch+sizeof(type)+sizeof(name), sizeof(value));

		//if((strcmp(type, "DO")==0 || strcmp(type, "PO")==0) || strcmp(type, "DV")==0))
		if(	strcmp(type, "DO")==0 || 
			strcmp(type, "PO")==0 || 
			strcmp(type, "DV")==0 )
		{
				printlog(LOG_D, "Updating digital value %s '%s'", 
					name, value);

				domino_data_t dd;
				strNcpy(dd.ip, ip_f, sizeof(dd.ip));
				strNcpy(dd.pname, name, sizeof(dd.pname));
				snprintf(dd.path,sizeof(dd.path), 
					DRIVER_PATH "/ODC%s/%s", uid_f, name);
			if (strcmp(name,"E")!=0) 
			{				
				fifo_output(dd.path, domino_set_digital_output, &dd);
				create_output_value_file(dd.path, value);

				char path[64];
				snprintf(path, sizeof(path), 
					CONF_PATH "/ODC%s/%s", uid_f, name);
				create_output_info_file(path,1);
			} 
		}
		else if (strcmp(type, "AV")==0)
		{
				printlog(LOG_D, "Updating AV %s '%s'", name, value);

				domino_data_t dd;
				strNcpy(dd.ip, ip_f, sizeof(dd.ip));
				strNcpy(dd.pname, name, sizeof(dd.pname));
				snprintf(dd.path,sizeof(dd.path), 
					DRIVER_PATH "/ODC%s/%s", uid_f, name);
			if (strcmp(name,"E")!=0) 
			{				
				fifo_output(dd.path, domino_set_analog_output, &dd);
				create_output_value_file(dd.path, value);

				char path[64];
				snprintf(path, sizeof(path), 
					CONF_PATH "/ODC%s/%s", uid_f, name);
				create_output_info_file(path,1);
			} 
		}
		else if((strcmp(type, "DI")==0)||(strcmp(type, "AI")==0))	
		{
			domino_update_files("ODC01", uid_f, type, name, value);
		}
		else if((strcmp(type, "XX")==0))	
		{
			// Disabled
		}
		else
		{
			printlog(LOG_N, "Invalid port type: %s (%s = %s)", 
				type, name, value);
			//fflush(stdout);
		}

		//notify_port_change(name,value);
		pch = strtok (NULL, " \n\r");
	}
}
// }}}

int main(int argc, char *argv[])
{
	int status;
	puts("Starting daemon");
	fflush(stdout);
	if(fork()==0)
	{
		domino_process_incoming_messages();	
	}
	else
	{
		int last_ndevs=0;
		for(;;)
		{
			int ndevs = domino_check_device_list();

			if(ndevs > last_ndevs)
				domino_manage_output_files();

			last_ndevs = ndevs;

			sleep(5);
		}
	}
	
   return 0;
} 


