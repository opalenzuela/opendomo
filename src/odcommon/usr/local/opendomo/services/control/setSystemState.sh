#!/bin/sh
#desc:Change state
#package:odcommon
#type:local

STDIR="/etc/opendomo/states"
SCENESDIR="/etc/opendomo/scenes"
RLVLFILE="/var/opendomo/run/runlevel"

if test `whoami` = "root"; then
	echo "#ERR This script cannot be called as root!"
	exit 1
fi

if ! test -z "$1"; then
	# Script called with a parameter. Possible cases:
	# 1. The parameter is the code of a state
	if test -d "$STDIR/$1" && test -x "$STDIR/$1"; then
		if test -f $STDIR/$1/.name; then
			SNAME=`cat $STDIR/$1/.name`
		else
			SNAME="$1"
		fi
		#echo "#INF: Changing to state [$SNAME]"
		OUTPUT="CGI" /etc/init.d/rcS $1 start  >/dev/null
		#echo "#INF: Operation successful"
		
		#NEW IN 0.9.6: if there is a scene with the same name, we call it!
		if test -f $SCENESDIR/state$1.conf; then
			/usr/local/opendomo/setScene.sh state$1 > /dev/null
		fi
	fi

	# 2. The parameter is just a service
	if test -x "/etc/init.d/$1"; then
        if test "$2" = "on"
        then
        	#echo "# Starting $1"
			/etc/init.d/$1 start >/dev/null
        fi
        if test "$2" = "off"
        then
        	#echo "# Stopping $1"
			/etc/init.d/$1 stop >/dev/null
     fi
fi
fi

if test -f "$RLVLFILE"; then
	RL=`cat $RLVLFILE`
fi

echo "#> Change state"
echo "list:`basename $0`"
for i in $STDIR/*; do
	if test -d $i && test -x $i; then
		KEY=`basename $i`
		if test -z "$LIST"; then
			LIST="$KEY"
		else
			LIST="$LIST,$KEY"
		fi
		DESC="$KEY"
		if test -f $i/.name; then
			DESC=`cat $i/.name`
		fi 
		if test "$KEY" = "$RL"; then
			echo "	-$KEY	$DESC	state sel	"
		else
			echo "	-$KEY	$DESC	state	"
		fi
	fi
done
echo "actions:"
if test -x /usr/local/opendomo/manageSystemStates.sh; then
	echo "	manageSystemStates.sh	Manage system states"
fi
echo
echo "#> Available services"
echo "form:`basename $0`"
for i in /etc/init.d/* 
do
	BN=`basename $i`
	if test "$BN" != "rcS"; then
		if test -x $i && test -f $i
		then
			NAME=`grep '#desc' $i | cut -f2- -d:`
			if test -z "$NAME"
			then
				NAME="$BN"
			fi
			PREFIX=`echo $BN| cut -f1 -d_`
			if test "$PREFIX" != "$BN"; then
				if test "$PREVPREFIX" != "$PREFIX"; then
					#echo "	$PREFIX	$PREFIX	separator	"
					PREVPREFIX="$PREFIX"
				fi
				TYPE="sub"
			else
				TYPE=""
			fi
			if $i status >/dev/null 2>/dev/null
			then
					echo "	$BN	$NAME	subcommand[on,off]	on"
			else
					echo "	$BN	$NAME	subcommand[on,off]	off"
			fi
		fi
	fi
done
echo "actions:"
echo
