#!/bin/sh
#desc:Delete control device
#package:odcontrol
#type:local

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

CFGPATH="/etc/opendomo/control"
CTRLPATH="/var/opendomo/control"


if test -z "$1"; then
	echo "# No device specified"
	echo "#INFO: No device specified"
	exit 1
fi


# Stopping service
/etc/init.d/odcontrol stop >/dev/null

for file in "$@"
do
	rm -fr $CTRLPATH/$file
	rm -fr $CFGPATH/$file
	rm -fr $CFGPATH/$file.conf
done

# Starting service
/etc/init.d/odcontrol start >/dev/null

/usr/local/opendomo/manageControlDevices.sh

