#!/bin/sh
#desc:Delete user
#type:multiple
#package:odbase

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

if ! test -z $1 || test "$1" != "admin"; then
    sudo userdel $1
else
    echo "#ERRO User $1 can't be deleted"
fi
manageUsers.sh
