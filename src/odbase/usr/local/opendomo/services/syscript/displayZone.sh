#!/bin/sh
#desc:Display zone
#type:local
#package:odcommon

# Copyright(c) 2012 OpenDomo Services SL. Licensed under GPL v3 or later

ZONEDIR=/etc/opendomo/zones
ODDIR=/usr/local/opendomo

# If nothing is specified, list the zones
if test -z "$1"; then
	echo "#> Zones"
	echo "list:`basename $0`	map"
	cd $ZONEDIR
	for i in *; do
		if test "$i" = "*"; then
			echo "#WARN No zones were defined. Press Manage zones to add yours"
		else
			. $ZONEDIR/$i
			echo "	-$i	$description	zone"
			echo "#TIP($i) $description"
		fi
	done
	echo "actions:"
	if test -x /usr/local/opendomo/manageZones.sh; then
		echo "	manageZones.sh	Manage zones"
	fi
	if test -x /usr/local/opendomo/configureMap.sh; then
		echo "	configureMap.sh	Configure map"
	fi
	echo
	exit 0
fi


export ZONE=$1
export SELECTEDZONE=$1
CTRLCFG="/etc/opendomo/control"

# If there is a parameter, but that zone does not exist...
if ! test -f $ZONEDIR/$1; then
	PORT=`echo $1 | sed 's/-/\//'`
	# ... The parameter might be the command to activate a port
	if test -f $CTRLCFG/$PORT.info; then
		$ODDIR/listControlPorts.sh $1 $2 >/dev/null
		. $CTRLCFG/$PORT.info
		export ZONE="$zone"
		export SELECTEDZONE="$zone"
	fi
fi

Z="$ZONEDIR/$ZONE"
if test -f $Z; then
	. $Z
	# List all the elements in the zone:
	ITEMFOUND=0
	# 1. ODHAL ports (ODControl, Arduino, X10, etc)
	if test -x $ODDIR/listControlPorts.sh; then
		. $ODDIR/listControlPorts.sh
		echo
		ITEMFOUND=1
	fi
	
	. /etc/opendomo/zones/$ZONE
	#2. Map with the zone highlighted
	echo 
	echo "#> $description"
	echo "list:displayZone.sh	map"
	cd $ZONEDIR
	for i in *; do
		. $ZONEDIR/$i
		if test $i = $ZONE; then
			echo "	-$i	$description	zone current"
		else
			echo "	-$i	$description	zone"		
		fi
		echo "#TIP($i) $description"
	done
	echo "actions:"
	echo "	displayZones.sh	Display all zones"
	echo 

	
	#3. ODVISION cameras
	if test -x $ODDIR/cameraShow.sh; then
		. $ODDIR/cameraShow.sh
		echo
		ITEMFOUND=1
	fi
	
	if test "$ITEMFOUND" -eq "0"; then
		echo "# No controllable items found"
	fi
else
	echo "#ERR Invalid zone [$Z]"
fi
echo

