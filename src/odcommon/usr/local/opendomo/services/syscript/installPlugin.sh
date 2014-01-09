#!/bin/sh
#type:local
#package:odcommon

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

REPOFILE="/var/opendomo/tmp/repo.lst"

echo "#INFO Installing plugin $1"

/usr/local/bin/bghell "sudo /usr/local/bin/plugin_add.sh $1"

echo