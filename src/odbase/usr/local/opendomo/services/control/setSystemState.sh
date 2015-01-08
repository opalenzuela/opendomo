#!/bin/sh
#desc:Change state
#package:odbase
#type:local

# Copyright(c) 2015 OpenDomo Services SL. Licensed under GPL v3 or later

# Variables
DAEMONSDIR="/usr/local/opendomo/daemons"
CONFIGSDIR="/etc/opendomo/states"
STATEDIR="$CONFIGSDIR/$1"
STATEPID="/var/opendomo/run/state.pid"
INITDIR="/etc/init.d"

# With parameter, execute changes
if test -z $2; then
    if ! test -z $1; then
        # Change system state
        logevent changestate "$1" "Changing state to [$1] "
        cd $DAEMONSDIR
        for daemon in *; do
	    # Start/Stop service in state
            if test -f $STATEDIR/$daemon; then
                logevent debug service "Starting service [$daemon]"
                sudo odservice $daemon status &>/dev/null || sudo odservice $daemon start
            else
                logevent debug service "Stopping service [$daemon]"
                sudo odservice $daemon status &>/dev/null && sudo odservice $daemon stop
            fi
        done
        # Change state in pidfile
        echo "$1" > $STATEPID
    fi
fi

# Always reload parameters and see interface
STATESDIR="/etc/opendomo/states"
INITRDDIR="/etc/init.d"
CURSTATE=`cat $STATEPID`

# Available states
echo "#> Change state"
echo "list:`basename $0`"
cd $STATESDIR
for state in *; do
    if test -d $state && [ $state != "idle" ]; then
        if   [ "$state" = "$CURSTATE" ]; then
            echo "	-$state	$state	state selected"
        else
            echo "	-$state	$state	state"
        fi
    fi
done
echo "action:"
echo "	manageSystemStates.sh	Manage system states"
echo

/usr/local/opendomo/setDaemons.sh
