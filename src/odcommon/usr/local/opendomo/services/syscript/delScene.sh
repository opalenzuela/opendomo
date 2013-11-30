#!/bin/sh
#desc:Delete scene
#package:odcommon
#type:local

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

CFGPATH="/etc/opendomo/scenes"


if test -z "$1"; then
	echo "# No scene specified"
	echo "#INFO: No scene specified"
	exit 1
fi


for file in "$@"
do
	if test -f "$CFGPATH/$file.conf"; then
		if rm -f "$CFGPATH/$file.conf"; then
			echo "#INFO: Secene deleted [$file]"
		else
			echo "#ERR: Cannot delete"
			exit 2
		fi
	fi
done


/usr/local/opendomo/manageScenes.sh
