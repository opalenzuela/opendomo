#!/bin/sh
#desc:Set all
#package:odhal

TAG="$1"
VALUE="$2"

if test -z "$TAG" || test -z "$VALUE"; then
	echo "#ERR: Missing parameters"
	exit 1
else

	cd /etc/opendomo/control

	for p in `grep tags -r * | grep $TAG | cut -f1 -d.`; do
		echo "$VALUE" > /var/opendomo/control/$p
	done
	exit 0
fi
