#!/bin/sh
#desc:Edit system state
#package:odbase
#type:local

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

DAEMONSDIR="/usr/local/opendomo/daemons"
CONFIGSDIR="/etc/opendomo/states"
TEMPSTATE="/var/opendomo/tmp/state.tmp"
INITRDDIR="/etc/init.d"


# Check if state exist and see form
if ! test -d "$CONFIGSDIR/$1" || test -z "$1"; then
	echo "#> Configure states"
	echo "#ERR Need specify state"
	echo
	exit 1
else
	i=1
	if ! test -z "$2"; then
		# Commands received
		STATE="$1"
		VALUES="$@"
		cd $DAEMONSDIR
		for service in *; do
			let i=i+1
			VAL=`echo $VALUES | cut -f$i -d' '`
			echo "Evaluating $service in $VAL"
			case "$VAL" in
				on)
					echo " $service should be on in $STATE "
					touch "$CONFIGSDIR/$STATE/$service"
				;;
				off)
					echo " $service will not be on in $STATE "
					rm -fr "$CONFIGSDIR/$STATE/$service"
				;;
			esac
		done
	fi
	# Check service status and print services
	echo "#> Configure state: [$1]"
	echo "form:`basename $0`"
	echo "	code	Code	hidden	$1"

	cd $DAEMONSDIR
	for service in *; do
		# Check service information and status
		DESC=`grep "# Short-Description" $service | cut -f2 -d:`

		if test -f "$CONFIGSDIR/$1/$service"; then
			STATUS=on
		else
			STATUS=off
		fi
		echo "	$service	$DESC	list[on,off] switch	$STATUS"
	done
	echo "action:"
	echo "	manageSystemStates.sh	Back"
	echo "	editSystemState.sh	Save changes"
	if test "$1" != "active"; then
		echo "	delSystemState.sh	Delete"
	fi
	echo

	# Save current state in temporal file
	echo "$1" >$TEMPSTATE
fi
