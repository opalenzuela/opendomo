#!/bin/sh
DATE=`date +%s`
LASTMIN=`cat /proc/loadavg | cut -f1 -d' '`
INTVAL=`echo $LASTMIN | cut -f1 -d.`

MAXCPU="80"
if test "$INTVAL" -gt "$MAXCPU"; then
	/bin/logevent warning system "CPU usage over [$MAXCPU]"
fi
echo "$DATE $LASTMIN"	
