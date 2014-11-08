#!/bin/sh
#desc:Change state
#package:odbase
#type:local

# Checks
if test `whoami` = "root"; then
    echo "#ERR This script cannot be called as root!"
    exit 1
fi

# Always check blacklist
sudo odstatesmng blacklist

# With parameter, execute changes
if test -z $2; then
    if ! test -z $1; then
        # Change system state
        sudo odstatesmng state $1 &>/dev/null
    fi
else
    # Start / Stop service, Ignoring boot services
    if ! test -z $1 && [ "$2" = "on" ]; then
        sudo odstatesmng service $1 start &>/dev/null
    elif ! test -z $1 && [ "$2" = "off" ]; then
        sudo odstatesmng service $1 stop  &>/dev/null
    fi
fi

# Always reload parameters and see interface
STATESDIR="/etc/opendomo/states"
DAEMONSDIR="/usr/local/opendomo/daemons"
INITRDDIR="/etc/init.d"
CURSTATE=`cat /var/opendomo/run/states.pid`
STATES=`ls -1 "$STATESDIR" | grep -v $CURSTATE | tr '\n' "," | sed 's/.$//'`

# Available states
echo "#> Change state"
echo "list:`basename $0`"
cd $STATESDIR
for state in *; do
    if test -d $state; then
        if [ "$state" = "$CURSTATE" ]; then
            echo "	$state	$state	state selected"
	else
            echo "	-$state	$state	state"
        fi
    fi
done
echo "action:"
echo

# Available services
echo "#> Available services"
echo "form:`basename $0`"

cd $DAEMONSDIR
for service in *; do
    # Check service information and status
    DESC=`grep "Name:" $service | awk -F: '{print$2}'`
    test -z "$DESC" && DESC=`grep "# Short-Description" $service | cut -f2 -d:`

    if ./$service status >/dev/null 2>/dev/null ; then
        STATUS=on
    else
        STATUS=off
    fi
    echo "	$service	$DESC	subcommand[on,off]	$STATUS"
done
echo "action:"
echo "	manageSystemStates.sh	Manage system states"
echo
