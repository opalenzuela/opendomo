#!/bin/sh
#desc:Add tag
#type:local
#package:odcommon

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

TAGDIR=/etc/opendomo/tags/
if ! test -d $TAGFILE; then
	mkdir $TAGFILE
fi


TAG=`echo $1 | grep "^[a-zA-Z0-9_ ]\+$"`

# Si se ha pasado como argumento, añadimos el TAG
if test -n "$TAG"; then
	if ! grep $TAG $TAGFILE > /dev/null; then
		echo "$1" >> $TAGDIR/$TAG
	fi
	echo "#INFO: Tag created"
	/usr/local/opendomo/manageTags.sh
else
	echo "#> New"
	echo "form:`basename $0`"
	echo "	tag	Name	text"
fi
echo



