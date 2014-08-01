#!/bin/sh
#desc:Manage users
#type:multiple
#package:odcommon


# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

CONFIGDIR="/etc/opendomo/udata"
USER="$1"

# If $1 modify user
if ! test -z $USER && test -f $CONFIGDIR/$USER.info; then
    /usr/local/opendomo/modifyUser.sh $USER
else
    # No user selected, see users list
    cd $CONFIGDIR
    echo "#> Available users"
    echo "list:`basename $0`	selectable"
    for user in `ls *.info | cut -f1 -d.`; do
        FULLNAME=`grep ^FULLNAME= $user.info | sed 's/\"//g' | cut -f2 -d= `
        echo "	-$user	$FULLNAME	user"
    done
    echo "action:"
    echo "	modifyUser.sh	Add / Modify"
    echo "	deleteUser.sh	Delete"
    echo
fi
