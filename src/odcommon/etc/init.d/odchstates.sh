#! /bin/sh
### BEGIN INIT INFO
# Provides:          states
# Required-Start:
# Required-Stop:
# Should-Start:      glibc
# Default-Start:     2 3 4 5
# Default-Stop:
# Short-Description: Change default opendomo state
# Description:       Change default opendomo state
#
### END INIT INFO
### Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later


. /lib/init/vars.sh
. /lib/lsb/init-functions

do_start () {
	DEFSTATE="active"
	STATEEXEC="/usr/local/bin/changestate.sh"

	log_daemon_msg "Loading default state"
	echo -n " $DEFSTATE"
	log_end_msg $?
	$STATEEXEC change $DEFSTATE
}

case "$1" in
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
