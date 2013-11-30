#!/bin/sh
DATE=`date +%s`
INFO=`grep MemFree /proc/meminfo| sed 's/[^0-9]//g'`
MINFREE="1000"
if test "$INFO" -lt "$MINFREE"; then
	/bin/logevent warning system "Free memory under [$MINFREE]"
fi
echo "$DATE $INFO"
