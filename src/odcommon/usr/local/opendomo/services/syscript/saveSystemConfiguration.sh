#!/bin/sh
#desc:Save system configuration
#package:odcommon
#type:multiple

TMPFILE="/var/opendomo/tmp/savestatus"
CONFEXE="/usr/local/bin/manage_conf.sh"
IMGEXE="/usr/local/sbin/mkrootfs"
SAVEPLUGINS=`cat $TMPFILE`

# Select option
if test "$SAVEPLUGINS" = "on" 
then
	echo "#INF Configuration and plugins saved"
	$CONFEXE save
	sudo $IMGEXE custom
else
	echo "#INF Configuration saved"
	$CONFEXE save
fi
