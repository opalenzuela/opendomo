#!/bin/sh
#desc:Edit system state
#package:oddistro
#type:local

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

DAEMONSDIR="/usr/local/opendomo/daemons"
CONFIGSDIR="/etc/opendomo/states"
TEMPSTATE="/var/opendomo/tmp/state.tmp"

case $2 in
  on )
	# If $2 is on, $1 is a service, check and change status in state
	STATE=`cat $TEMPSTATE`

	if test -e $DAEMONSDIR/$1; then
		touch $CONFIGSDIR/$STATE/$1
	fi
  ;;
  off )
	# If $2 is on, $1 is a service, check and change status in state
	STATE=`cat $TEMPSTATE`

	if test -e $DAEMONSDIR/$1; then
		rm $CONFIGSDIR/$STATE/$1
	fi
  ;;
  * )
	# Check if state exist and see form
	if ! test -d $CONFIGSDIR/$1 || test -z $1; then
		echo "#> Configure states"
		echo "#ERR Need specify state"
		echo
		exit 1
	else
		# Save current state in temprola file
		echo "$1" >$TEMPSTATE

		# Check service status and print services
		echo "#> Configure state ($1)"
		echo "form:`basename $0`"

		cd $DAEMONSDIR
		for service in *; do
			DESC=`grep "# Short-Description" $service | cut -f2 -d:`
			if test -f $CONFIGSDIR/$1/$service; then
				STATUS=on
			else
				STATUS=off
			fi
			echo "	$service	$DESC	subcommand[on,off]	$STATUS"
		done
		echo "action:"
		echo "	manageSystemStates.sh	Back"
		echo
	fi
  ;;
esac
