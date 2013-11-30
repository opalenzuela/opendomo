#!/bin/sh
#desc:Turn all lights off
#package:odcontrol
#type:local

if test -x /usr/local/bin/setallports.sh; then
	/usr/local/bin/setallports.sh light off
	echo "#INFO:Lights are off"
else
	echo "#ERR: Missing file [setallports.sh]"
	exit 1
fi
echo
