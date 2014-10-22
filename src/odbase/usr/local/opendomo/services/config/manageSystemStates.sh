#!/bin/sh
#desc:Manage system states
#type:local
#package:odbase

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

STATESDIR=/etc/opendomo/states
TEMPSTATE=/var/opendomo/tmp/state.tmp
STATE="$1"

# Always check blacklist
changestate.sh blacklist

# $1 only can be a state selected to configure
if ! test -z $STATE && test -d $STATESDIR/$STATE; then
    /usr/local/opendomo/editSystemState.sh $STATE
else
    rm $TEMPSTATE 2>/dev/null

    # If $1 don't exist, show all states
    cd $STATESDIR
    echo "#> Available states"
    echo "list:`basename $0`	selectable"
    for state in *; do
        echo "	-$state	$state	tag"
    done
    echo "action:"
    echo "	addSystemState.sh	Add"
    echo
fi
