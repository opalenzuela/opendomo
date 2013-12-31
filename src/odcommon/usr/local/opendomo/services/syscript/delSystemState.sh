#!/bin/sh
#desc:Delete state
#type:local
#package:odcommon

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

STATESDIR=/etc/opendomo/states
STATE=$1

if test -z $STATE; then
	echo "#> Delete states"
	echo "#ERR No system state specified"
	exit 1
else
	if [ "$STATE" = "active" ]; then
		echo "#ERR State 'active' can't be deleted"
	else
		echo "#INFO State $STATE deleted"
		rm -r $STATESDIR/$STATE
	fi

	# Return to manage states
	/usr/local/opendomo/manageSystemStates.sh
fi
