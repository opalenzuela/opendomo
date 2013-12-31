#! /bin/sh
#desc: Opendomo configuration manager

# Variables
DAEMONSDIR="/usr/local/opendomo/daemons"
CONFIGSDIR="/etc/opendomo/states"
STATE=$2
STATEDIR=$CONFIGDIR/$STATE

# Checks
if test -z $2; then
	echo "#ERR Needs select state"
	exit 1
fi

#TODO: Se necesita implementar $1 change y $1 service start/stop

echo "#INFO: Changing $1 state ..."
if ! test -d $STATEDIR; then
	echo "#ERR State $2 don't exist"
	exit 1
else
	for state in $DAEMONSDIR; do
		if test -f $DAEMONSDIR/$state && test -f $STATEDIR/$state; then
			# This is a opendomo service, started with admin user
			sudo -u admin $DAEMONSDIR/$state restart >/dev/null 2>/dev/null

		elif test -h $DAEMONSDIR/$state && test -f $STATEDIR/$state; then
			# This is a system service controled by plugins
			$DAEMONSDIR/$state restart >/dev/null 2>/dev/null

		else
			# This services is stop in this state
			$DAEMONSDIR/$state stop >/dev/null 2>/dev/null

		fi
	done
fi
