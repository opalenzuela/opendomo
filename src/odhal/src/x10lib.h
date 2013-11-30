
#ifndef __X10LIB_H__
#define __X10LIB_H__

#include <usb.h>

#define X10_DELAY 500000
#define R_EP	0x81
#define W_EP   0x02
#define LIBUSB_TIMEOUT	2500
#define MAXDEVS 32


/* globals */
struct usb_device *x10_dev_list[MAXDEVS];
usb_dev_handle *x10_dev_handle_list[MAXDEVS];

extern int x10_dev_idx;


int x10_init();
int x10_release();
int x10_send_command(int dev_idx, char* command);

#endif

