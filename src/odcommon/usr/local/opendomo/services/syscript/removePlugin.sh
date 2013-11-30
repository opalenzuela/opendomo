#!/bin/sh
#type:local
#package:odcommon

REPOFILE="/var/opendomo/tmp/repo.lst"
URLFILE=`grep ^$1 $REPOFILE| cut -f2 -d';' |sort |head -n1`
DEPS=`grep ^$1 $REPOFILE| cut -f4 -d';' |sort |head -n1`

echo "INFO: Removing plugin $1"

#TODO Support ZIP or other formats as well
wget $URLFILE -O /var/opendomo/tmp/$1.tar.gz

# Execute as root
#TODO Do not remove packages needed by other plugins
#TODO add "unattended"
/usr/local/opendomo/bin/bgshell "apt-get remove $DEPS && cd / && tar -zxvf /var/opendomo/tmp/$1.tar.gz"

echo