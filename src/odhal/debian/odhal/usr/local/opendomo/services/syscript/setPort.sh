#!/bin/sh
#desc:Set port value
#type:local
#package:odhal

PORT=/var/opendomo/control/$(echo $1 | sed -e 's/\./\//' -e 's/_/\//' )
VALUE="$2"
if test -z "$PORT" || test -z "$VALUE"; then
	echo "#ERR Data missing"
	echo
	exit 1
fi

if ! test -e "$PORT"
then
	echo "#ERR Invalid port $PORT"
	echo
	exit 1
else
	echo "# Setting value $VALUE to $PORT"
	echo $VALUE > $PORT
	echo
fi


