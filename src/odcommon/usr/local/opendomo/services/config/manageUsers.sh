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
    echo "list:`basename $0`	selectable iconlist"
    # No users configured yet, see a message
    test -z `ls --hide="admin.info" | head -c1` && echo "#INFO No users configured yet"

    for user in `ls *.info | cut -f1 -d.`; do
		# Do not present "Admin" as a user
		if test "$user" != "admin"
		then
			FULLNAME=`grep ^FULLNAME= $user.info | sed 's/\"//g' | cut -f2 -d= `
			echo "	-$user	$FULLNAME	user"
		fi
    done
    echo "action:"
    echo "	modifyUser.sh	Add / Modify"
    echo "	deleteUser.sh	Delete"
    echo
fi
