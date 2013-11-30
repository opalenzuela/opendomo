#!/bin/sh
#desc:Show memory usage
#type:local
#package:odcgi_flot

#TODO Allow date selection
MEMFILE="/var/log/stats/loadmem.stats"


if test -f $MEMFILE; then
	for d in `sed -e 's/^/[/' -e 's/$/]/' -e 's/ /,/' $MEMFILE`; do
		if test -z "$MEMDATA"; then
			MEMDATA="$d"
		else
			MEMDATA="$MEMDATA,$d"
		fi
	done

	echo "#> Memory usage"
	echo "graph:`basename $0`"
	echo "	mem	MEM	[$MEMDATA]"
else
	echo "#WARN: Statistic files not found"
fi
echo
