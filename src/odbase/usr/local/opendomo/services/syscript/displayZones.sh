#!/bin/sh
#desc:Display zone
#type:local
#package:odbase

# Copyright(c) 2012 OpenDomo Services SL. Licensed under GPL v3 or later

ZONEDIR=/etc/opendomo/zones
ODDIR=/usr/local/opendomo

# If nothing is specified, list the zones
if test -z "$1"; then
	echo "#> Zones"
	echo "list:displayZone.sh	map"
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
else
	echo "#WARN Unexpected parameter $1"
	echo
fi

