#!/bin/sh
#type:local
#package:odcommon

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

REPOFILE="/var/opendomo/apt/repository.lst"
PLUGINSDIR="/var/opendomo/apt/plugins"
QUEUEFILE="/var/opendomo/apt/queue"

if test -z "$1"; then
    echo "#ERRO You must specify the plugin ID"
    exit 2
else
    # Sending deps to queue and download file
    DOWNURL=`grep $1 $REPOFILE  | tail -n1 | cut -f2 -d";"`
    PLUGDEPS=`grep $1 $REPOFILE | tail -n1 | cut -f4 -d";"`
    echo -n "$PLUGDEPS" >> $QUEUEFILE

    cd $PLUGINSDIR
    /usr/local/bin/download.sh $DOWNURL

    echo "#INFO Plugin [$1] sent to install"
    /usr/local/opendomo/managePlugins.sh
fi
echo
