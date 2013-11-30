#!/bin/sh
#desc:Rotate and pack log files
#package:distro

# This script is launched ONCE a day, and should 
# leave this folder clean.
DATE=`date +%Y%m%d`
cd /var/log
mkdir -p stats/$DATE

# Wait until the zero-second
while test `date +%S` != "00"
do
	sleep 1
done

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

mv /var/log/messages /var/log/stats/$DATE/messages.log
touch /var/log/messages
/etc/init.d/syslog reload > /dev/null

cd /var/log/stats/$DATE/
# Packing
gzip *.log
gzip *.err

VARS=`ls *.stats | sed 's/\.stats//g'`
for v in $VARS; do
	tar -cvf $v.tar $v.h* && rm $v.h* && gzip $v.tar
done
