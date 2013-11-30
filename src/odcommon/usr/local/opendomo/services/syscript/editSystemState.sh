#!/bin/sh
#desc:Edit system state
#package:oddistro
#type:local

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

STDIR="/etc/opendomo/states"

if test -z "$1"; then
	echo "#> System states"
	echo "list:`basename $0`"
	for d in $STDIR/*; do
		BN=`basename $d`
		if test -f $d/.name
		then
			b=`cat $d/.name`
			echo "	-$BN	$b	state"
		fi
	done
#	echo "actions:"
#	echo "	manageSystemStates.sh	Edit"
#	echo
else
	STAT=`echo $1 | cut -f1 -d.`
	SERVICE=`echo $1 | cut -f2 -d.`
	if test "$STAT" = "boot" ; then
		echo "#WARN Read-only directory"
	fi
	if test "$2" = "on"; then
		if test -x "/etc/init.d/$SERVICE"; then
			if ! test -s /$STDIR/$STAT/$SERVICE; then
				ln -s /etc/init.d/$SERVICE /$STDIR/$STAT/$SERVICE
			fi
		fi
	else
		if test "$2" = "off"; then
			# Setting service to "off"
			rm /$STDIR/$STAT/$SERVICE 2>/dev/null
		fi
	fi
	echo "#> Services active in [$STAT]"
	echo "form:`basename $0`"
	for i in /etc/init.d/*; do
		if test -x $i; then
			BN=`basename $i`
			if test "$BN" != "rcS"; then
				NAME=`grep '#desc' $i | cut -f2- -d:`
				if test -f $STDIR/$STAT/$BN; then
					STATE="on"
				else
					STATE="off"
				fi
				echo "	$STAT.$BN	$NAME	subcommand[on,off]	$STATE"
			fi
		fi
	done
	echo "action:"
	echo "	manageSystemStates.sh	Manage system states"
fi
echo
