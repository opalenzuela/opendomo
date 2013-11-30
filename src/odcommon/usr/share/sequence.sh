#!/bin/sh
TIME=`date +%R`
WEEKDAY=`date +%u`
setport(){
	PORT=`echo $1 | sed 's/./\//'`
	if test -f "/var/opendomo/control/$PORT"; then
		cat $2 > /var/opendomo/control/$PORT
	else
		echo "#ERR Invalid port $PORT"
	fi
}
setall(){
	echo "#WARN function disabled"
}
wait(){
	echo "# Sleeping $1 seconds"
	sleep $1
}
play(){
	/usr/bin/aplay $1
}

