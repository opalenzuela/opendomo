#!/bin/sh
#desc:Delete user
#type:multiple
#package:odcommon

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

USER=$1
if test -z $1; then
    echo "#ERRO User is not selected"
else
    if test "$1" = "admin"
    then
        echo "#ERRO Administrator cannot be deleted"
    else
        sudo manageusers.sh del $USER
    fi
fi
manageUsers.sh
