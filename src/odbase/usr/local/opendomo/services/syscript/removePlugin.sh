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

        echo "#INFO Plugin [$1] sent to remove"
        /usr/local/opendomo/managePlugins.sh
    else
        echo "#ERRO Plugin $plugin not installed"
    fi
fi
