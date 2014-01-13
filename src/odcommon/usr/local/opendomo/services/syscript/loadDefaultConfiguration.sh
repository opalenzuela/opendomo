#!/bin/sh
#desc:Load default configuration
#package:odcommon
#type:local

CONFEXE="/usr/local/bin/manage_conf.sh"
IMGEXE="sudo mkrootfs"
TMPFILE="/var/opendomo/tmp/savestatus"
SAVEPLUGINS=`cat $TMPFILE`
CTCHANGE="/mnt/odconf/images/dfchange.img.gz"

if [ "$SAVEPLUGINS" = "on" ]; then
	echo "#INF Restore system to default state"
	$CONFEXE restore
	if test -f $CTCHANGE; then
		$IMGEXE restore
	fi
else
	echo "#INF Restore default configuration"
	$CONFEXE restore
fi
