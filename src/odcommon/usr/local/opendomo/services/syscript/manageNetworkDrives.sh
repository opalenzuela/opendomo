#!/bin/sh
#desc:View network drives
#package:odcommon
#type:local

CONF="/etc/opendomo/nfsdrives"

echo "#> Current network drives"
echo "list:`basename $0`"
DRIVES=`ls $CONF/* 2>/dev/null`
for i in $DRIVES; do
	LABEL=`basename $i`
	PATH=`cat $i`
	echo "	-$LABEL	$LABEL	netdrive"
done
echo "actions:"
echo "	addNetworkDrive.sh	Add network drive"
echo "	delNetworkDrive.sh	Remove network drive"
echo
