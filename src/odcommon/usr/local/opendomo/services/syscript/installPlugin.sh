#!/bin/sh
#type:local
#package:odcommon

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

REPOFILE="/var/opendomo/tmp/repo.lst"

if ! test -f "$REPOFILE"
then
	echo "#ERROR You must execute Manage Plugins first"
	exit 1
fi


if test -z "$1"
then
	echo "#ERROR You must specify the plugin ID"
	exit 2
else
	SIZE=`grep ^$1 $TMPDIR/repo.lst | sort | head -n1 | cut -f7 -d';' `
	AVAIL=`/usr/local/bin/get_mem_free.sh | cut -f1 -d' '`
	if test "$SIZE" -gt "$AVAIL"
	then
		echo "#ERROR You need at least [$SIZE] Bytes free"
		exit 3
	else
		echo "#INFO Installing plugin $1"
		/usr/local/bin/plugin_add.sh $1 >/dev/null 2>/dev/null
	fi
	echo "list:$0"
	echo "actions:"
	echo "	managePlugins.sh	Back"
fi
echo
