#!/bin/sh
#desc:Manage tags
#type:local
#package:odcommon

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

TAGDIR=/etc/opendomo/tags
if ! test -d $TAGFILE; then
	mkdir $TAGFILE
fi


if test -z "$1"; then
	echo "#WARN: No option selected"
	/usr/local/opendomo/manageTags.sh
	exit 0
fi

cd $TAGDIR
for tag in "$@"; do
	if test -f $tag; then
		rm $tag
	fi
done

echo "#INFO: Tag deleted"
/usr/local/opendomo/manageTags.sh
