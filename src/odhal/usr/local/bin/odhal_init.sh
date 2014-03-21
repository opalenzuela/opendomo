#!/bin/sh
IPADDRESS="$1"
BNAME="ODC$2"
if test -z "$1"; then
	echo "usage: $0 ip_address [UID]"
	exit 1
fi
if test -z "$BNAME"; then
	BNAME=`grep "$IPADDRESS" /var/opendomo/domino_devices.conf | cut -f2 -d:`
	if test -z "$BNAME" 
	then
		echo "ERROR: $IPADDRESS not found in /var/opendomo/domino_devices.conf:"
		cat /var/opendomo/domino_devices.conf
		exit 2
	fi
	BNAME="ODC$BNAME"
fi

TMPFILE=/tmp/$$.portlist
if ping $IPADDRESS >/dev/null
then
	echo lst | nc $IPADDRESS 1729 | cut -f2 -d: > $TMPFILE 
else
	echo "$IPADDRESS does not respond"
	exit 3
fi
/bin/logevent notice odhal "Processing new ODControl [$IPADDRESS]" $0

sleep 60 # leave time for the driver to create the files

for p in `cat $TMPFILE`
do
	cd /etc/opendomo/tags
	echo lst $p | nc $IPADDRESS 1729 | sed -e 's/ /\n/g' -e 's/:/=/g'  > /tmp/$IPADDRESS-$p.data
	echo "$p added"
	tag=""
	tag=`grep tag= /tmp/$IPADDRESS-$p.data | cut -f2 -d=`
	# this cannot be done. Some chars (&) are invalid to be included as values
	# . /tmp/$IPADDRESS-$p.data
	if ! test -z "$tag" && test "$tag" != "0"; then
		tname=`ls $tag* | head -n1` 2>/dev/null
		if ! test -z "$tname"; then
			INFOFILE="/etc/opendomo/control/$BNAME/$p.info"
			if test -f "$INFOFILE"; then
				chmod +w $INFOFILE
				sed -i "s/tag='none'/tag='$tname'/" $INFOFILE
				chmod -w $INFOFILE
			else
				echo "File $INFOFILE not found"
			fi
		else
			echo "invalid tag $z"
		fi
	else
		echo "empty tag"
	fi
	usleep 100
done
rm /var/opendomo/tmp/listcontrolports* 
rm $TMPFILE
