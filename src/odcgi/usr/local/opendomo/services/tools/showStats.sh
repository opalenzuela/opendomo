#!/bin/sh
#desc:Show statistics
#type:local
#package:odcgi_flot

if test -z "$1"; then
	echo "#> Select variable"
	echo "list:`basename $0`"
	cd /var/log/stats
	VARS=`ls | cut -f1 -d. | uniq`
	for n in $VARS; do
		if ! test -d $n
		do
			echo "	-$n	$n	$n"
		done
	done
	echo
	exit 0
fi

echo "#> Data"
echo "graph:`basename $0`"

for f in $1 $2 $3 $4; do 
	/usr/local/opendomo/services/syscript/generateStats.sh stats $f
	n=`echo $f | sed 's/-/_/'`
	VARIABLE="/var/log/stats/$f.stats"
	if test -f $VARIABLE; then
		for d in `sed -e 's/^/[/' -e 's/$/]/' -e 's/ /,/' $VARIABLE`; do
			if test -z "$DATA"; then
				DATA="$d"
			else
				DATA="$DATA,$d"
			fi
		done

		echo "	$n	$n	[$DATA]"
	fi
done
echo "actions:"
echo "	showStats.sh	Back to list"
echo
