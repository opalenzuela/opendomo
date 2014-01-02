#!/bin/sh

PKGID="$1"
REPOFILE="/var/opendomo/tmp/repo.lst"
DEPS=`grep ^$1 $REPOFILE| cut -f4 -d';' |sort |head -n1`
URLFILE=`grep ^$1 $REPOFILE| cut -f2 -d';' |sort |head -n1`
PROGRESS="/var/opendomo/run/$PKGID.progress"

LFILE="/var/opendomo/tmp/$1.tar.gz"

echo "00" > $PROGRESS

#TODO Support ZIP or other formats as well
if wget $URLFILE -O $LFILE --no-check-certificate
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
fi
echo "75" > $PROGRESS

cd /

/bin/tar -zxvf $LFILE > /var/opendomo/log/$PKGID.files
echo "100" > $PROGRESS