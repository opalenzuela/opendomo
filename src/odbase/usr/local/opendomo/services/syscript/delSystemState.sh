#!/bin/sh
#desc:Delete state
#type:local
#package:odcommon

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

STATESDIR=/etc/opendomo/states
TEMPSTATE="/var/opendomo/tmp/state.tmp"
STATE=$1

if ! test -z $STATE; then
    test $STATE != active && rm -r $STATESDIR/$STATE
elif test -f $TEMPSTATE; then
    STATE=`cat $TEMPSTATE`
    test $STATE != active && rm -r $STATESDIR/$STATE
else
    echo "#> Delete states"
    echo "#ERR No system state specified"
    exit 1
fi

# Return to manage states
/usr/local/opendomo/manageSystemStates.sh
