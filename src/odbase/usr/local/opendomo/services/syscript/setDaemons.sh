#!/bin/sh
#desc:Set Daemons
#package:odbase
#type:local

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

# Variables
DAEMONSDIR="/usr/local/opendomo/daemons"

echo "#> Available services"
echo "form:setDaemons.sh	autosubmit"
VALUES="$@"
cd $DAEMONSDIR
i=0
for service in *; do
    # If we passed as parameters
    if ! test -z "$VALUES"; then
        let i=i+1
        VAL=`echo $VALUES | cut -f$i -d' '`
    fi

    # Check service information and status
    DESC=`grep "# Short-Description" $service | cut -f2 -d:`

    if ./$service status >/dev/null 2>/dev/null ; then
        STATUS=on
        # Only if status is ON and we requested OFF, we execute
        test "$VAL" = "off" && sudo odservice $service stop >/dev/null 2>/dev/null
    else
        STATUS=off
        # Or the other way around
        test "$VAL" = "on" && sudo odservice $service start >/dev/null 2>/dev/null
    fi
    echo "	$service	$DESC	list[on,off] switch	$STATUS"
done
echo
