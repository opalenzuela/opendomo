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
 * date: March 2011
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <sys/wait.h>

#include "x10lib.h"



int x10_dev_idx = -1;



// {{{ x10_init()
int x10_init()
{
   struct usb_bus *bus;
   struct usb_device *dev;

   usb_init();
   //usb_set_debug(100);
   usb_find_busses();
   usb_find_devices();

   x10_dev_idx = 0;

   for (bus = usb_get_busses(); bus; bus = bus->next)
   {
      for (dev = bus->devices; dev; dev = dev->next)
      {
         if (dev->descriptor.idVendor == 0x0bc7 &&
             dev->descriptor.idProduct == 0x0001)
         {
            x10_dev_list[++x10_dev_idx] = dev;
         }
      }
   }

	if(x10_dev_idx==0)
	{
		fprintf(stderr, "Error: Device not found\n");
		return -1;
	}

	x10_dev_handle_list[x10_dev_idx] = usb_open(x10_dev_list[x10_dev_idx]);
	if(!x10_dev_handle_list[x10_dev_idx])
	{
		fprintf(stderr, "Error: Can not open USB device\n");
		return -1;
	}
	
   if(usb_set_configuration(x10_dev_handle_list[x10_dev_idx], 1)!=0)
	{
		fprintf(stderr, "Error: Can not set USB configuration\n");
		return -1;
	}
   
	usb_detach_kernel_driver_np(x10_dev_handle_list[x10_dev_idx], 0);

   if(usb_claim_interface(x10_dev_handle_list[x10_dev_idx], 0)!=0)
	{
		fprintf(stderr, "Error: Can not claim ISB interface\n");
		return -1;
	}

	/*
   if(usb_set_altinterface(x10_dev_handle_list[x10_dev_idx], 3)!=0)
	{
		fprintf(stderr, "Error: Can not set alt interface\n");
		return -1;
	}
	*/
	//usb_clear_halt(x10_dev_handle_list[x10_dev_idx], X_EP);
	usb_clear_halt(x10_dev_handle_list[x10_dev_idx], W_EP);
   usb_clear_halt(x10_dev_handle_list[x10_dev_idx], R_EP);
   
	return 0;
}
// }}}

// {{{ x10_release()
int x10_release()
{
	int i;
	for(i=1; i<=x10_dev_idx; i++)
	{
		usb_release_interface(x10_dev_handle_list[i], 0);
   	usb_close(x10_dev_handle_list[i]);
   	x10_dev_handle_list[i] = NULL;
	}

	x10_dev_idx=0;
	return 0;
}
// }}}

// {{{ X10_send()
int x10_send(int dev_idx, char *buffer, size_t len)
{
	usb_dev_handle *dev = x10_dev_handle_list[dev_idx];
	if(!dev)
	{
		fprintf(stderr, "Error: device not found: %d\n", dev_idx);
		return -1;
	}	

	return usb_bulk_write(dev, W_EP, buffer, len, LIBUSB_TIMEOUT);
}
// }}}

// {{{ X10_recv()
int x10_recv(int dev_idx, char *buffer, size_t len)
{
	usb_dev_handle *dev = x10_dev_handle_list[dev_idx];
	
	return usb_bulk_read(dev, R_EP, buffer, len, LIBUSB_TIMEOUT);
}
// }}}

// {{{ x10_get_address_code()
char x10_get_address_code(char *address)
{
	char code = 0;

	// House Code
	if     (toupper(address[0])=='A')
		code=0x60;
	else if(toupper(address[0])=='B')
		code=0xE0;
	else if(toupper(address[0])=='C')
		code=0x20;
	else if(toupper(address[0])=='D')
		code=0xA0;
	else if(toupper(address[0])=='E')
		code=0x10;
	else if(toupper(address[0])=='F')
		code=0x90;
	else if(toupper(address[0])=='G')
		code=0x50;
	else if(toupper(address[0])=='H')
		code=0xD0;
	else if(toupper(address[0])=='I')
		code=0x70;
	else if(toupper(address[0])=='J')
		code=0xF0;
	else if(toupper(address[0])=='K')
		code=0x30;
	else if(toupper(address[0])=='L')
		code=0xB0;
	else if(toupper(address[0])=='M')
		code=0x00;
	else if(toupper(address[0])=='N')
		code=0x80;
	else if(toupper(address[0])=='O')
		code=0x40;
	else if(toupper(address[0])=='P')
		code=0xC0;

	// Device Code
	int n = atoi(address+1);

	switch(n)
	{
		case 1:
		code ^=0x06;
		break;

		case 2:
		code ^=0x0E;
		break;

		case 3:
		code ^=0x02;
		break;

		case 4:
		code ^=0x0A;
		break;

		case 5:
		code ^=0x01;
		break;

		case 6:
		code ^=0x09;
		break;

		case 7:
		code ^=0x05;
		break;

		case 8:
		code ^=0x0D;
		break;

		case 9:
		code ^=0x07;
		break;

		case 10:
		code ^=0x0F;
		break;

		case 11:
		code ^=0x03;
		break;

		case 12:
		code ^=0x0B;
		break;

		case 13:
		code ^=0x00;
		break;

		case 14:
		code ^=0x08;
		break;

		case 15:
		code ^=0x04;
		break;

		case 16:
		code ^=0x0C;
		break;
	}

	return code;
}
// }}}

// {{{ x10_get_function_code()
char x10_get_function_code(char address_code, char *function)
{
	char acode = address_code & 0xF0;
	char fcode = 0;

	if     (strcasecmp(function, "ALLUOFF")==0)
		fcode = 0x00;
	else if(strcasecmp(function, "ALLLON")==0)
		fcode = 0x01;
	else if(strcasecmp(function, "ON")==0)
		fcode = 0x02;
	else if(strcasecmp(function, "OFF")==0)
		fcode = 0x03;
	else if(strcasecmp(function, "DIM")==0)
		fcode = 0x04;
	else if(strcasecmp(function, "BRIGHT")==0)
		fcode = 0x05;
	else if(strcasecmp(function, "ALLLOFF")==0)
		fcode = 0x06;
	else if(strcasecmp(function, "EXTC")==0)
		fcode = 0x07;
	else if(strcasecmp(function, "HREQUEST")==0)
		fcode = 0x08;
	else if(strcasecmp(function, "HACK")==0)
		fcode = 0x09;
	else if(strcasecmp(function, "PSDIM1")==0)
		fcode = 0x0A;
	else if(strcasecmp(function, "PSDIM2")==0)
		fcode = 0x0B;
	else if(strcasecmp(function, "EXTD")==0)
		fcode = 0x0C;
	else if(strcasecmp(function, "STATUSON")==0)
		fcode = 0x0D;
	else if(strcasecmp(function, "STATUSOFF")==0)
		fcode = 0x0E;
	else if(strcasecmp(function, "STATUSREQ")==0)
		fcode = 0x0F;
		
	return acode ^ fcode;
}
// }}}

// {{{ x10_send_command()
int x10_send_command(int dev_idx, char* command)
{
	char address[8];
	char function[16];
	sscanf(command, "%8s %16s", address, function);

	char apacket[2];
	char fpacket[2];

	char acode = x10_get_address_code(address);
	char fcode = x10_get_function_code(acode, function);


	// -- HEADER --
	// Bit:	   7   6   5   4   3   2   1   0
	// Header:	< Dim amount    >   1  F/A E/S
	// 
	// Dim amount = 0, F/A = 0, E/S = 0
	apacket[0]=0x04;
	apacket[1]=acode;

	// Dim amount = 0, F/A = 1, E/S = 0
	fpacket[0]=0x06;
	fpacket[1]=fcode;

	x10_send(dev_idx, apacket, sizeof(apacket));
	usleep(X10_DELAY);
	x10_send(dev_idx, fpacket, sizeof(fpacket));

	return 0;
}
// }}}



/* test function */
#ifdef MAIN
int main()
{
	int s;
	char buffer[8];

	if(x10_init()!=0)
	{
		fprintf(stderr, "Error: x10_init()\n");
		return -1;
	}

	if(fork()==0)
	{
		for(;;)	
		{
			s = x10_recv(1, buffer, 1);
			if(s==1)
			{
				printf("%x\n", buffer[0]);
			}
		}
	}
	
	for(;;)
	{
		/*
		x10_send_command(1, "a2 on");
		sleep(5);
		x10_send_command(1, "a2 off");
		sleep(5);
		*/
	}

	wait(NULL);

	return 0;
}
#endif


