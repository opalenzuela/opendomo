#!/bin/sh

if test -z "$1"
then
    echo "usage: $0 pluginID"
    echo "usage: $0 /path/to/plugin.file"
    echo
    exit 1
fi

if test -f "$1"
then 
    #If the parameter is a file
    PKGID=`basename $1| cut -f1 -d'-'`
else
    PKGID="$1"
fi

REPOFILE="/var/opendomo/tmp/repo.lst"
if ! test -f "$REPOFILE"
then
	echo "#ERROR $REPOFILE does not exist. Launch managePlugins.sh and try again"
	exit 2
fi
DEPS=`grep ^$1 $REPOFILE| cut -f4 -d';' |sort |head -n1`
URLFILE=`grep ^$1 $REPOFILE| cut -f2 -d';' |sort |head -n1`
PROGRESS="/var/opendomo/run/$PKGID.progress"
STORAGE="/mnt/odconf/plugins"

if ! test -d $STORAGE
then
    mkdir -p $STORAGE
fi

LFILE="$STORAGE/`basename $URLFILE`"

echo "00" > $PROGRESS


if wget $URLFILE -O $LFILE -q --no-check-certificate
then
    SHA1=`grep ^$1 $REPOFILE| cut -f5 -d';' |sort |head -n1`
    #TODO Check SHA1
else
    exit 2
fi
echo "50" > $PROGRESS

#Execute as root
if ! test -z "$DEPS"; then
    apt-get install $DEPS -y 
else
	echo "No dependencies found"
fi
echo "75" > $PROGRESS

cd /
echo $LFILE | grep ".tar.gz" - && /bin/tar -zxvf $LFILE > /var/opendomo/log/$PKGID.files
echo $LFILE | grep ".zip" - && /usr/local/bin/unzip -o $LFILE > /var/opendomo/log/$PKGID.files
rm $PROGRESS