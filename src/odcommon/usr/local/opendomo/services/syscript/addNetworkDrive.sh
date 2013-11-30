#!/bin/sh
#desc:Add network drive
#package:odcommon
#type:local

CONF="/etc/opendomo/nfsdrives"

if test -z "$3"; then
	echo "form:`basename $0`"
	echo "	host	Host IP	text:[0-9.]*	$host"
	echo "	route	Path	text:[a-z0-9\/.]*	$route"
	echo "	label	Label	text:[a-z0-9]*	$label"
	echo 
else
	if test -f $CONF/$3; then
		echo "#ERR There's another drive with the same name"
	else
		echo "$1:$2" > $CONF/$3
		echo "# Network drive added."
		echo "# You should save the configuration and reboot"
	fi
fi
