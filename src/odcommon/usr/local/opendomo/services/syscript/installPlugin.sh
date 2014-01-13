#!/bin/sh
#type:local
#package:odcommon

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

REPOFILE="/var/opendomo/tmp/repo.lst"

if test -z "$1"
then
	echo "#ERROR You must specify the plugin ID"
else
	echo "#INFO Installing plugin $1"
	/usr/local/bin/bgshell "/usr/local/bin/plugin_add.sh $1"
fi
echo