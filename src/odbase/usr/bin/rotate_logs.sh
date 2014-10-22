#!/bin/sh
#desc:Rotate and pack log files
#package:odcommon

# This script is launched ONCE a day, and should 
# leave this folder clean.
DATE=`date +%Y%m%d`
LOGPATH="/var/opendomo/log"
cd $LOGPATH
mkdir -p stats/$DATE

# Wait until the zero-second
echo -n "Waiting for zero-second..."
while test `date +%S` != "00"
do
	sleep 1
done
echo "DONE!"

for i in *.*; do
	# Only files
	if test -f "$i"; then
		mv $i stats/$DATE/ && touch $i && chown admin $i
	fi
done

cd stats
for i in *.*; do
	if test -f "$i"; then
		mv $i $DATE/ && touch $i && chown admin $i
	fi
done
cd ..



cd $LOGPATH/stats/$DATE/
# Packing
gzip *.log
gzip *.err

VARS=`ls *.stats | sed 's/\.stats//g'`
for v in $VARS; do
	tar -cvf $v.tar $v.h* && rm $v.h* && gzip $v.tar
done
