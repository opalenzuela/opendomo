#!/bin/sh
#desc:Delete sequence step
#type:local
#package:odcommon

# Copyright(c) 2012 OpenDomo Services SL. Licensed under GPL v3 or later

SEQPATH="/etc/opendomo/sequences"

if test -z "$1"; then
	echo "#WARN No sequence specified"
	exit 1
fi

FILE=`echo $1 | cut -f1 -d-`
STEPS=`echo $@ | sed -e "s/$FILE-//g"`

if test -f $SEQPATH/$FILE; then
	for step in `echo $STEPS`; do
		SEDCMD="$SEDCMD -e ${step}d"
	done     
	sed -i $SEDCMD $SEQPATH/$FILE
	echo "#INFO Step removed"
else
	echo "#ERR File not found"
	echo 2
fi

/usr/local/opendomo/manageSequenceSteps.sh $FILE
