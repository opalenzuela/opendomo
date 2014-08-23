#!/bin/sh
#type:local
#package:odcommon

PLUGINSDIR="/var/opendomo/plugins"
PLUGIN="$1"

# Creating remove file for opendomo-apt
if test -z $1; then
    echo "#ERRO You need select plugin first"
else
    cd $PLUGINSDIR
    if test -f $PLUGIN.files; then
        touch $PLUGIN.remove
    else
        echo "#ERRO Plugin $plugin not installed"
    fi
fi
