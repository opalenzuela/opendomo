#!/bin/sh
#desc:Event reception
#package:odbase
#type:local

# Copyright(c) 2015 OpenDomo Services SL. Licensed under GPL v3 or later

EHDIR="/etc/opendomo/eventhandlers"

if test -z "$1" || test -z "$2" || test -z "$3"
then
	echo "Usage: $0 module level message [archivo]"
	echo "	module=odbase|any plugin installed"
	echo "	level=error|warn|notice|debug|... (custom allowed)"
	echo "	message=description of the event"
	exit 0
fi

MODULE=$1
LEVEL=$2
MSG=$3

test -x $EHDIR/all-all && . $EHDIR/all-all $LEVEL $MODULE "$MSG" $4
test -x $EHDIR/$MODULE-all && . $EHDIR/$MODULE-all $LEVEL $MODULE "$MSG" $4
test -x $EHDIR/all-$LEVEL && . $EHDIR/all-$LEVEL  $LEVEL $MODULE "$MSG" $4
test -x $EHDIR/$MODULE-$LEVEL && . $EHDIR/$MODULE-$LEVEL $LEVEL $MODULE "$MSG" $4

# Tolerate inverted order as well (backwards compatibility)
test -x $EHDIR/$LEVEL-$MODULE && . $EHDIR/$LEVEL-$MODULE $MODULE $LEVEL "$MSG" $4
test -x $EHDIR/all-$MODULE && . $EHDIR/all-$MODULE  $MODULE $LEVEL "$MSG" $4
test -x $EHDIR/$LEVEL-all && . $EHDIR/$LEVEL-all $MODULE $LEVEL"$MSG" $4
