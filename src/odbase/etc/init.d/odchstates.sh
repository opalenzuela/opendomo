#! /bin/sh
### BEGIN INIT INFO
# Provides:          states
# Required-Start:    $all
# Required-Stop:
# Should-Start:
# Default-Start:     1 2 3 4 5
# Default-Stop:
# Short-Description: Change default opendomo state
# Description:       Load default state in boot process
#
### END INIT INFO
### Copyright(c) 2015 OpenDomo Services SL. Licensed under GPL v3 or later


. /lib/init/vars.sh
. /lib/lsb/init-functions

PIDFILE="/var/opendomo/run/state.pid"
UIDFILE="/etc/opendomo/uid"
SYSSTATUS="/var/www/data/status.json"

do_start () {
	if test -f $PIDFILE; then
		STATE=`cat $PIDFILE`
	else
		if test -f $UIDFILE ; then
			STATE="active"
		else
			STATE="idle"
		fi
	fi
	STATEDIR="/etc/opendomo/states/$STATE"
	
    # Create sysstatus and pid
    echo "{\"status\":\"$STATE\"}" > $SYSSTATUS 
	chown admin:admin $SYSSTATUS
    echo "$STATE" > $PIDFILE && chown admin:admin $PIDFILE

    # Start services in state
    cd $STATEDIR
    for daemon in *; do
        test -f $daemon && odservice $daemon start
    done
}

do_background() {

}

case "$1" in
	background)
		do_background
		;;
    start|"")
        do_start
        ;;
    restart|reload|force-reload)
        echo "Error: argument '$1' not supported" >&2
        exit 3
        ;;
    status)
        echo "Error: argument '$1' not supported" >&2
        exit 3
        ;;
    stop)
        # No-op
        ;;
    *)
        echo "Usage: odchstates.sh [start]" >&2
        exit 3
        ;;
esac

:
