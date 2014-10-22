#! /bin/sh
### BEGIN INIT INFO
# Provides:          states
# Required-Start:    $all
# Required-Stop:
# Should-Start:
# Default-Start:     S
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
	DAEMONDIR="/usr/local/opendomo/daemons"
	INITRDDIR="/etc/init.d/"

	# Starting boot service
	cd $DAEMONDIR
	for service in *; do
		if test -x "/etc/init.d/$service"; then
			RUNLEVEL=`cat $service | grep "# Default-Start:" | awk '{print $3}'`

			if [ "$RUNLEVEL" = "S" ]; then
				service $service start
			fi
		fi
	done

	# Starting services in default state
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
