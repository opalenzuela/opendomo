#!/bin/sh
#desc:Manage users
#type:multiple
#package:odbase

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

# If $1 modify user
if ! test -z $1; then
    /usr/local/opendomo/userModify.sh $1

elif [ `grep -c "/bin/bash" /etc/passwd | grep -v "root:" | grep -v "admin:"` -eq 2 ]; then
    # The list is emply
    echo "#INFO No users configured yet"

else
    # No user selected, see users list ignoring admin
    cd $CONFIGDIR
    echo "#> Available users"
    echo "list:`basename $0`    selectable iconlist"
    IFS=$'\n'; for user in `grep "/bin/bash" /etc/passwd | grep -v "root:" | grep -v "admin:"`; do
        USER=`grep $user /etc/passwd | awk -F: '{print$1}'`
        INFO=`grep $user /etc/passwd | awk -F: '{print$5}'`
        echo "  -$USER  $INFO   user"
    done
    echo "action:"
    echo "      userModify.sh   Add"
    echo
fi
