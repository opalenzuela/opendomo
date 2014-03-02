#!/bin/sh
#desc:Delete user
#type:multiple
#package:odcommon

USER=$1
if ! test -z $1; then
    echo "#INFO User $USER has been deleted"
    sudo manageusers.sh del $USER >/dev/null 2>/dev/null

    manageUsers.sh
else
    echo "#ERROR User is not selected"
fi
