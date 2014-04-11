#!/bin/sh
#desc:Add control device
#package:odcontrol
#type:local

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

CFGPATH="/etc/opendomo/control"
CTRLPATH="/var/opendomo/control"
MODELS="dummy"
PS=""
PORTS="/dev/dummy"

mkdir -p $CFGPATH

# micropik ports
if test -e /usr/bin/micropik
then
	MODELS="$MODELS, micropik:micropik"
	PS="`ls /dev/ttyS* 2>/dev/null | cut -c1-15` "
fi

# arduino ports
if test -e /usr/bin/arduino
then
	MODELS="$MODELS, arduino:arduino"
	PS="$PS `ls /dev/ttyU* 2>/dev/null | cut -c1-15`" 
fi

# x10 ports
if test -e /usr/bin/x10
then
	MODELS="$MODELS, x10:x10"
	PS="$PS 
	`/usr/bin/x10 -l 2>/dev/null`" 
fi

# ODControl2 ports
if test -e /usr/bin/domino
then
	MODELS="$MODELS, domino:ODControl"
fi


# list of all available ports
for p in $PS; do
	PORTS="$PORTS,$p:$p"
done


refresh=100
if test -f "$CFGPATH/$1.conf"
then
	. "$CFGPATH/$1.conf"
fi

echo "#> Control device"
echo "form:manageControlDevices.sh"

if echo $1 | grep ^ODC > /dev/null; then 
	echo "# Domino network devices can not be edited"
	echo "actions:"
	echo "	manageControlDevices.sh	Manage control devices"
else
	echo "	action	action	hidden	update"
	echo "	model	Board model	list[$MODELS]	$driver"
	echo "	port	Port	list[$PORTS]	$device"
	echo "	dirname	Name	text:[a-zA-Z0-9_]+	$dirname"
	echo "	refresh	Refresh	text.integer	$refresh"
	echo "	ip	IP address	text	$ip"	
	echo "	username	Username	text	$username"
	echo "	password	Password	text	$password"
	echo "actions:"
	echo "	manageControlDevices.sh	Save"
fi
echo
	



