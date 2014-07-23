#!/bin/sh
#type:local
#package:odcommon

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

REPOFILE="/var/opendomo/tmp/repo.lst"
PLUGINSDIR="/var/opendomo/apt/plugins"
QUEUEFILE="/var/opendomo/apt/queue"

if ! test -f "$REPOFILE"
then
	echo "#ERRO You must execute Manage Plugins first"
	exit 1
fi

if test -z "$1"
then
	echo "#ERRO You must specify the plugin ID"
	exit 2
else
	# Sending deps to queue and download file
	DOWNURL=`cat $REPOFILE | grep $1 | tail -n1 | cut -f2 -d";"`
	PLUGINDEPS=`cat $REPOFILE | grep $1 | tail -n1 | cut -f4 -d";"`
	echo -n "$PLUGINDEPS" >> $QUEUEFILE
	cd $PLUGINSDIR
	/usr/local/bin/download.sh $DOWNURL

	echo "actions:"
	echo "	managePlugins.sh	Back"
fi
echo
