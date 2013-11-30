#!/bin/sh
#desc:Recepción de eventos
#package:odevents
#type:local

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

# Este script recibe los eventos de la red OpenDomo con la siguiente estructura:
# logEvent.sh [nivel] [modulo] [mensaje]
# nivel=error|warn|notice|
# modulo=odcgi|odpkg|oddiscovery|... cualquier modulo instalado
# mensaje=Cadena de texto

EHDIR="/etc/opendomo/eventhandlers"

if test -z "$1" || test -z "$2" || test -z "$3"
then
	echo "Usage: $0 nivel modulo mensaje [archivo]"
	exit 0
fi


LEVEL=$1
MODULE=$2
MSG=$3

test -x $EHDIR/all-all && . $EHDIR/all-all $LEVEL $MODULE "$MSG" $4
test -x $EHDIR/$MODULE-all && . $EHDIR/$MODULE-all $LEVEL $MODULE "$MSG" $4
test -x $EHDIR/all-$LEVEL && . $EHDIR/all-$LEVEL  $LEVEL $MODULE "$MSG" $4
test -x $EHDIR/$MODULE-$LEVEL && . $EHDIR/$MODULE-$LEVEL $LEVEL $MODULE "$MSG" $4
test -x $EHDIR/odai_rules-$MODULE-$LEVEL && . $EHDIR/$MODULE-$LEVEL $LEVEL $MODULE "$MSG" $4

