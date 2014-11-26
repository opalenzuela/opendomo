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
    for user in `grep "/bin/bash" /etc/passwd | grep -v "root:" | grep -v "admin:"`; do
        username=`grep $user /etc/passwd | awk -F: '{print$1}'`
        INFO=`grep $user /etc/passwd | awk -F: '{print$5}'`
        echo "	-$username	$INFO	user"
    done
	if test -z "$username"; then
		echo "#INFO No users configured yet"
	fi
    echo "action:"
    echo "	userModify.sh	Add"
	echo "	userDelete.sh	Delete"
    echo
fi
