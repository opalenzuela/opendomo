#!/bin/sh
#desc:Manage system states
#type:local
#package:odbase

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

STATESDIR=/etc/opendomo/states
TEMPSTATE=/var/opendomo/tmp/state.tmp
STATE="$1"

# Always check blacklist (Disabled for error) Deprecated?
#odstatesmng blacklist 2>/dev/null 


rm $TEMPSTATE 2>/dev/null

# If $1 don't exist, show all states
cd $STATESDIR
echo "#> Available states"
echo "list:editSystemState.sh	selectable"
for state in *; do
	echo "	-$state	$state	tag"
done
echo "action:"
echo "	addSystemState.sh	Add"
echo

