#!/bin/sh
#desc:Delete user
#type:multiple
#package:odcommon

USER=$1
if ! test -z $1; then
    sudo manageusers.sh del $USER
    manageUsers.sh
else
    echo "#ERROR User is not selected"
fi
