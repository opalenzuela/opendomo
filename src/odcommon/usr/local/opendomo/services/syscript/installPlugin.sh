#!/bin/sh
#type:local
#package:odcommon

REPOFILE="/var/opendomo/tmp/repo.lst"

echo "#INFO Installing plugin $1"

/usr/local/bin/bghell "sudo /usr/local/bin/plugindecompress.sh $1"

echo