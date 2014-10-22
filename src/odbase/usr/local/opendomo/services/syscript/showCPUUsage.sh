#!/bin/sh
#desc:Show CPU usage
#type:local
#package:odbase_flot

#TODO Allow date selection
CPUFILE="/var/opendomo/log/stats/loadcpu.stats"


if test -f $CPUFILE; then
	for d in `sed -e 's/^/[/' -e 's/$/]/' -e 's/ /,/' $CPUFILE`; do
		if test -z "$CPUDATA"; then
			CPUDATA="$d"
		else
			CPUDATA="$CPUDATA,$d"
		fi
	done

	echo "#> CPU usage"
	echo "graph:`basename $0`"
	echo "	cpu	CPU	[$CPUDATA]"
else
	echo "#WARN: Statistic files not found"
fi
echo
