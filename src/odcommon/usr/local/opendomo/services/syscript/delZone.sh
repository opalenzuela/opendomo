#!/bin/sh
#desc:Delete zone
#type:local
#package:odcommon

ZONEDIR=/etc/opendomo/zones
ETCDIR=/var/opendomo/cgiroot
ODDIR=/usr/local/opendomo

DELETED=0
for zone in "$@"
do
	rm -f $ZONEDIR/$zone
	rm -f $ETCDIR/$zone
	DELETED=1
done

if test "$DELETED" = "1" ; then
	echo "#INFO: Zone deleted"
fi

/usr/local/opendomo/manageZones.sh
