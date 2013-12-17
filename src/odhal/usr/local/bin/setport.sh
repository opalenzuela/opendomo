#!/bin/sh
#desc:Set port
#package:odhal

CTRLPATH="/var/opendomo/control"
if test -e "$CTRLPATH/$1"; then
	if ! test -z "$2"; then
		echo "$2" > "$CTRLPATH/$1"
		sleep 1
	fi
fi
