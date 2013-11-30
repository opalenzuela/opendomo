#!/bin/sh

#desc:Delete files
#package:odcommon
#type:local

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later
# Isidro Catalan <icatalan@opendomo.com>


# ----------------------------------------------------------------------------
# desc: Delete files
# author: Isidro Catalan <skarvin@gmail.com>, http://www.opendomo.com
# date: May 2011
#
# CHANGES:
# ----------------------------------------------------------------------------

if [ "$1" = "confirmed" ]; then
	for i in `echo "$2" | tr "+" "\n"`; do
		echo -e "#INF: Deleting /media/$i"
		res="`echo $i | grep .mjpg`"
		if [ "$res" != "" ]; then
			jpg_file=`echo "$i" | sed 's/mjpg/jpg/'`
			fps_file=`echo "$i" | sed 's/mjpg/fps/'`			
			rm -f /media/$jpg_file
			rm -f /media/$fps_file			
		fi
		rm -f /media/$i
	done
else
    echo "#> Are you sure?"
    echo -e "form:`basename $0`
	status	Status	hidden	confirmed
	params	Params	hidden	$@
"
	echo
fi
