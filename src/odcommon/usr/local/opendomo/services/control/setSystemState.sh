#!/bin/sh
#desc:Change state
#package:odcommon
#type:local

CONFIGSDIR="/etc/opendomo/states"
DAEMONSDIR="/usr/local/opendomo/daemons"
CURSTATE=`cat /var/opendomo/run/states.pid`

if test `whoami` = "root"; then
	echo "#ERR This script cannot be called as root!"
	exit 1
fi

interface () {
  # Available states
  STATES=`ls -1 "$CONFIGSDIR" | grep -v $CURSTATE | tr '\n' "," | sed 's/.$//'`
  echo $STATES

  echo "#> Change state"
  echo "form:`basename $0`"
  echo "	change	Change state	hidden	change"
  echo "	state	Select state	list[$CURSTATE,$STATES]	$CURSTATE"
  echo
  echo "#> Available services"
  echo "form:`basename $0`"

  # Available services
  cd $DAEMONSDIR
  for service in *; do
	# Check service information
	DESC=`grep "# Short-Description" $service | cut -f2 -d:`
	if ./$service status >/dev/null 2>/dev/null ; then
		STATUS=on
	else
		STATUS=off
	fi
	echo "	$service	$DESC	subcommand[on,off]	$STATUS"
  done
  echo "action:"
  echo "	manageSystemStates.sh	Manage system states"
  echo
}



case $2 in
  on | off)
	# Change service
	changestate.sh service $1 $2
	interface
  ;;
  * )
	if [ "$1" = change ]; then
		# Change system state
		changestate.sh change $2
	else
		interface
	fi
  ;;
esac
