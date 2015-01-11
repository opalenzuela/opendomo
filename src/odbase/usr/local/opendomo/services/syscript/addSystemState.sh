#!/bin/sh
#desc:Add or modify state
#type:local
#package:odbase

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

STATESDIR="/etc/opendomo/states"
TEMPSTATE="/var/opendomo/tmp/state.tmp"

if ! test -z "$2" && [ "$1" = "new" ]; then
    # Creating new state
    mkdir  -p "$STATESDIR/$2"
    /usr/local/opendomo/editSystemState.sh "$2"

elif ! test -z "$2" && [ "$1" != "new" ]; then
    # Modify state
    if test -d "$STATESDIR/$1" && [ "$1" != "active" ]; then
        mv "$STATESDIR/$1" "$STATESDIR/$2"
        /usr/local/opendomo/editSystemState.sh "$2"
    else
        echo "#ERR State 'active' can't be modified"
        /usr/local/opendomo/manageSystemStates.sh
    fi

elif ! test -z $1 && test -z "$2"; then
    # Modify state
    echo "#> Modify state: [$1]"
    echo "form:`basename $0`"
    echo "	oldname	oldname	hidden	$1"
    echo "	name	Name	text	$STATE"
    echo
else
    # Add state
    echo "#> Add state"
    echo "form:`basename $0`"
    echo "	action	action	hidden	new"
    echo "	name	Name	text	$2"
    echo
fi
