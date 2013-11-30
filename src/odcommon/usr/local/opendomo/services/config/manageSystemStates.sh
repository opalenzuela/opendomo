#!/bin/sh
#desc:Manage system states
#type:local
#package:odcommon

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

STATEDIR=/etc/opendomo/states
ODDIR=/usr/local/opendomo

case $1 in
	"add"|"update")
		if test -z "$3"; then
			echo "#ERR Description field missing"
			exit 2
		fi
		echo "#INFO: State added successfully"
		mkdir -p $STATEDIR/$2		
		echo $3 > $STATEDIR/$2/.name
	;;
	"delete")
		echo "#INFO: State removed successfully"
		rm -fr $STATEDIR/$2
	;;
	*)	# Probablemente estamos intentando editar un estado
		if test -n "$1" && test -f $STATEDIR/$1; then
			. $ZONEDIR/$1
			echo "#> Add new state"
			echo "form:`basename $0`"
			echo "	action	action	hidden	update"
			echo "	key	Key	text	$2"
			echo "	name	Name	text"	$3

			exit 0
		fi
	;;
esac

cd $STATEDIR
echo "#> Available states"
echo "list:editSystemState.sh	selectable"
for s in *; do
	if test "$s" != "*" && test "$s" != "boot" ; then
		if test -z $STATES; then
			STATES="$s"
		else
			STATES="$STATES,$s"
		fi
		touch $s/.name
		NAME=`cat $s/.name`
		if test -z "$NAME"; then
			NAME=`basename $s`
		fi
		echo "	-$s	$NAME	state"
	fi
done
echo "action:"
echo "	addSystemState.sh	Add"
echo "	delSystemState.sh	Delete"
echo


