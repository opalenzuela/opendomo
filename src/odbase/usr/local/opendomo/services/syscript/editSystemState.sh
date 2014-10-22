#!/bin/sh
#desc:Edit system state
#package:oddistro
#type:local

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

DAEMONSDIR="/usr/local/opendomo/daemons"
CONFIGSDIR="/etc/opendomo/states"
TEMPSTATE="/var/opendomo/tmp/state.tmp"
INITRDDIR="/etc/init.d"

case $2 in
    on )
        # If $2 is on, $1 is a service, check and change status in state
        STATE=`cat $TEMPSTATE`
        test -e $DAEMONSDIR/$1 && touch $CONFIGSDIR/$STATE/$1
    ;;
    off )
        # If $2 is on, $1 is a service, check and change status in state
        STATE=`cat $TEMPSTATE`
        test -e $DAEMONSDIR/$1 && rm $CONFIGSDIR/$STATE/$1
    ;;
    * )
        # Check if state exist and see form
        if ! test -d $CONFIGSDIR/$1 || test -z $1; then
            echo "#> Configure states"
            echo "#ERR Need specify state"
            echo
            exit 1
        else
            # Check service status and print services
            echo "#> Configure state: [$1]"
            echo "form:`basename $0`"

            cd $DAEMONSDIR
            for service in `ls -1 * | grep -v .name`; do
                # Ignoring boot services and missing services
                if ! test -h "$DAEMONSDIR/$service" || test -x "$INITRDDIR/$service"; then
                    RUNLEVEL=`cat $service | grep "# Default-Start:" | awk '{print $3}'`

                        if [ "$RUNLEVEL" != "S" ]; then
                            test -f $service.name && DESC=`cat $service.name`
                            test -f $service.name || DESC=`grep "# Short-Description" $service | cut -f2 -d:`
                            if test -f $CONFIGSDIR/$1/$service; then
                                STATUS=on
                            else
                                STATUS=off
                            fi
                            echo "	$service	$DESC	subcommand[on,off]	$STATUS"
                        fi
                fi
            done
            echo "action:"
            echo "	manageSystemStates.sh	Back"
            if test "$1" != "active"; then
                echo "	addSystemState.sh	Rename"
                echo "	delSystemState.sh	Delete"
            fi
            echo

            # Save current state in temporal file
            echo "$1" >$TEMPSTATE
        fi
    ;;
esac
