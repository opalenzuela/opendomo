#!/bin/sh
#desc:Remove network drive
#package:distro
#type:local

CONF="/etc/opendomo/nfsdrives"

if ! test -z $1; then
	if test -f /$CONF/$1; then
		rm -f /$CONF/$1
		echo "# Network drive deleted"
		echo "# You should save the configuration and reboot"
	fi
fi

/usr/local/opendomo/manageNetworkDrives.sh
