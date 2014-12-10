#!/bin/sh
#desc:Manage users
#type:multiple
#package:odbase

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

# If $1 modify user
if ! test -z $1; then
    /usr/local/opendomo/userModify.sh $1

else
    # No user selected, see users list ignoring admin
    cd $CONFIGDIR
    echo "#> Available users"
    echo "list:manageUsers.sh	selectable iconlist"
	cd /home/
    for user in *; do
        INFO=`grep ^$user /etc/passwd | cut -f5 -d:`
		fullname=`echo $INFO | cut -f1 -d'<'`
		
		if test $user = "admin"; then
			echo "	-$user	$fullname	admin	$INFO"
		else
			echo "	-$user	$fullname	user	$INFO"
		fi
    done
	if test -z "$fullname"; then
		echo "#INFO No users configured yet"
	fi
    echo "action:"
    echo "	userModify.sh	Add"
	echo "	userDelete.sh	Delete"
    echo
fi
