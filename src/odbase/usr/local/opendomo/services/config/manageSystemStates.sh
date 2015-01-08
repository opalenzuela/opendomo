k#!/bin/sh
#desc:Manage system states
#type:local
#package:odbase

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

STATESDIR=/etc/opendomo/states
TEMPSTATE=/var/opendomo/tmp/state.tmp
STATE="$1"

rm $TEMPSTATE 2>/dev/null

# If $1 don't exist, show all states
cd $STATESDIR
echo "#> Available states"
echo "list:editSystemState.sh	selectable"
for state in *; do
    [ $state =! "idle" ] && echo "	-$state	$state	tag"
done
echo "action:"
echo "	addSystemState.sh	Add / Rename"
echo "	delSystemState.sh	Delete"
echo
echo "#TIP Press and hold to select state"
echo
