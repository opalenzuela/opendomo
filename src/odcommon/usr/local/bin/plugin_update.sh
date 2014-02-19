#!/bin/sh
#desc:Update all plugin files
# This script will update the installed plugins, downloading from the repository
# only the newer files.

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

if test -z "$1"
then
    echo "usage: $0"
    echo
    exit 1
fi


STORAGE="/mnt/odconf/plugins"
if ! test -d $STORAGE
then
    mkdir -p $STORAGE
fi

# First we make sure the directory exists
mkdir -p /var/opendomo/plugins/
cd /var/opendomo/plugins/ || exit 2

# Then we create a list with all the installed plugins
PLIST=""
for i in *.info
do	
	if test -f $i
	then
		f=`echo $i | cut -f1 -d'.'`
		PLIST="$PLIST $f"
	fi
done
cd /

echo "# Updating $PLIST ..."

# The repository index file must exist. 
# Otherwise it will trigger a self-explanatory error.
REPOFILE="/var/opendomo/tmp/repo.lst"
if ! test -f "$REPOFILE"
then
	echo "#ERROR $REPOFILE does not exist. Launch managePlugins.sh and try again"
	exit 2
fi
for PKGID in $PLIST
do
	LVER=`cat /var/opendomo/plugins/$PKGID.version` 2>/dev/null
	RVER=`grep ^$PKGID $REPOFILE| cut -f1 -d';' |sort |head -n1|cut -f1 -d. |cut -f2 -d-`
	
	if test -z "$RVER" || test -z "$LVER"
	then
		echo "# Missing version information for [$PKGID]:"
		echo "# Remote: $RVER - Local: $LVER "
	else
		if test "$RVER" -gt "$LVER"
		then
			echo "# Remote version is newer"
			/usr/local/bin/plugin_add.sh $PKGID
		fi
	fi

done
