#!/bin/sh
#type:local
#package:odcommon

PLUGINSDIR="/var/opendomo/plugins"
PLUGIN="$1"

# Creating remove file for opendomo-apt
if test -z $PLUGIN; then
    echo "#ERRO You need select plugin first"
else
    cd $PLUGINSDIR
    if test -f $PLUGIN.version; then
        touch $PLUGIN.remove
    else
        echo "#ERRO Plugin is not installed yet"
    fi
fi
