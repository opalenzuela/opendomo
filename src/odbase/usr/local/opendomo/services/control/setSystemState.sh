#!/bin/sh
#desc:Change state
#package:odbase
#type:local

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

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
       cd $DAEMONSDIR
       for daemon in *; do
           # Start/Stop service in state
           if test -f $STATEDIR/$daemon; then
               sudo odservice $daemon start 2>/dev/null
           else
               sudo odservice $daemon stop  2>/dev/null
           fi
       done
       # Change state in pidfile
       echo "$1" > $STATEPID
    fi
else
    # Start / Stop service
    if ! test -z $1 && [ "$2" = "on" ]; then
        sudo odservice $1 start &>/dev/null
    elif ! test -z $1 && [ "$2" = "off" ]; then
        sudo odservice $1 stop  &>/dev/null
    fi
fi

# Always reload parameters and see interface
STATESDIR="/etc/opendomo/states"
INITRDDIR="/etc/init.d"
CURSTATE=`cat $STATEPID`
#STATES=`ls -1 "$STATESDIR" | grep -v $CURSTATE | tr '\n' "," | sed 's/.$//'`

# Available states
echo "#> Change state"
echo "list:`basename $0`"
cd $STATESDIR
for state in *; do
    if test -d $state; then
        if [ "$state" = "$CURSTATE" ]; then
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
