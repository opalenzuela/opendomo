#!/bin/sh
#desc:Generate statistics
#package:odcommon
#type:local
# Params:
# $1 = day (YYMMDD), corresponding to /var/log/stats/YYMMDD/ existent folder
# $2 = varname, corresponding to /var/log/stats/YYMMDD/varname.h??

if test -z "$1"; then
	DAY="stats"
else
	DAY="stats/$1"
fi

if ! test -d /var/log/$DAY; then
	DAY="stats" # Current day "alive" stats
fi


if test -z "$2" || ! test -e /var/log/$DAY/$2.h00; then
	cd /var/log/$DAY
	vars=`ls *.h?? | sed 's/\.h..//g' | uniq`
else
	vars="$2"
fi

cd /var/log/$DAY

hours="00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23"

for v in $vars; do
	# Generating the hourly stats
	rm $v.stats 2>/dev/null
	for h in $hours; do
		if test -f $v.h$h; then
			avg=`cut -f2 -d' ' $v.h$h | /usr/local/bin/average` 2>/dev/null
			if ! test -z "$avg" ; then
				echo "$h $avg" >> $v.stats
			fi
		fi
	done
	if test -f $v.stats; then
		# Daily average (to build weekly and monthly statistics)
		cut -f2 -d' ' $v.stats | grep -e [0-9] | average > $v.davg 2>/dev/null
	fi
done


