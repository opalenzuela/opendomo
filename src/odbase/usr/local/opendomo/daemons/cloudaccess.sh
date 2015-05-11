#!/bin/sh
### BEGIN INIT INFO
# Provides:          cloudaccess
# Required-Start:    
# Required-Stop:
# Should-Start:      
# Default-Start:     1 2 3 4 5
# Default-Stop:      0 6
# Short-Description: Cloud access service
# Description:       Provide access to the cloud services and from Internet to the local network
#
### END INIT INFO

# Copyright(c) 2015 OpenDomo Services SL. Licensed under GPL v3 or later

. /lib/lsb/init-functions
DESC="Cloud access service"
PIDFILE="/var/opendomo/run/cloudaccess.pid"
TMPFILE="/var/opendomo/tmp/cloudaccess.tmp"
UIDFILE="/etc/opendomo/uid"
EXTERNALPORT="9325"

do_background() {
	echo "ON" >$PIDFILE
	uid=`cat  $UIDFILE`
	ODVER=`apt-cache show odbase | grep Version| cut -f2 -d' '`
	EMAIL=`grep admin: /etc/passwd | awk -F: '{print$5}' | cut -f2 -d"<" | cut -f1 -d">"`
	SYSTEMIP=`/sbin/ifconfig eth0 | grep "inet addr" | cut -f2 -d: | cut -f1 -d' '`

	test -f /usr/bin/upnpc && upnpc -a $SYSTEMIP 80 $EXTERNALPORT TCP
	
	while test -f $PIDFILE
	do
		URL="http://cloud.opendomo.com/activate/index.php?UID=$uid&VER=$ODVER&MAIL=$EMAIL&PORT=$EXTERNALPORT&INTERNALIP=$SYSTEMIP"
		wget -q -O $TMPFILE $URL 2>/dev/null
		sleep 600
	done
	
	# Service stopped here:
	URL="http://cloud.opendomo.com/activate/index.php?UID=$uid&MAIL=$EMAIL&ACTION=STOP"
	wget -q -O $TMPFILE $URL 2>/dev/null		
	
	test -f /usr/bin/upnpc && upnpc -d $EXTERNALPORT TCP
}

do_start(){
	log_action_begin_msg "Starting $DESC"
	test -f $PIDFILE || $0 background > /dev/null &
	log_action_end_msg $?
}

do_stop () {
	log_action_begin_msg "Stopping $DESC"	
	rm $PIDFILE 2>/dev/null
	log_action_end_msg $?
}

do_status () {
	if test -f $PIDFILE; then
		echo "$DESC is running"
		exit 0
	else
		echo "$DESC is not running"
		exit 1
	fi
}

case "$1" in
	background)
		do_background
		;;
	start)
		do_start
		;;
	restart|reload|force-reload)
		do_stop
		do_start
		exit 3
		;;
	stop)
		do_stop
	;;
	status)
		do_status
		exit $?
		;;
	*)
		echo "Usage: $0 [start|stop|restart|status]" >&2
		exit 3
		;;
esac
