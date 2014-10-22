#!/bin/sh
#desc:Event reception
#package:odcommon
#type:local

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

EHDIR="/etc/opendomo/eventhandlers"

if test -z "$1" || test -z "$2" || test -z "$3"
then
	echo "Usage: $0 level module message [archivo]"
	echo "	level=error|warn|notice|debug"
	echo "	module=odcgi|odpkg|oddiscovery|any plugin or package installed"
	echo "	message=description of the event"
	exit 0
fi

LEVEL=$1
MODULE=$2
MSG=$3

test -x $EHDIR/all-all && . $EHDIR/all-all $LEVEL $MODULE "$MSG" $4
test -x $EHDIR/$MODULE-all && . $EHDIR/$MODULE-all $LEVEL $MODULE "$MSG" $4
test -x $EHDIR/all-$LEVEL && . $EHDIR/all-$LEVEL  $LEVEL $MODULE "$MSG" $4
test -x $EHDIR/$MODULE-$LEVEL && . $EHDIR/$MODULE-$LEVEL $LEVEL $MODULE "$MSG" $4
#TODO remove the following line in future versions:
test -x $EHDIR/odai_rules-$MODULE-$LEVEL && . $EHDIR/$MODULE-$LEVEL $LEVEL $MODULE "$MSG" $4

