#! /bin/sh
### BEGIN INIT INFO
# Provides:          first_init
# Required-Start:
# Required-Stop:
# Should-Start:      glibc
# Default-Start:     S
# Default-Stop:
# Short-Description: First opendomo configuration file
# Description:       This script is exec only in first opendomo distro init
#
### END INIT INFO
### Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later


. /lib/init/vars.sh
. /lib/lsb/init-functions

SYSCONFDIR="/mnt/odconf/sysconf"
DEFCONF="/mnt/odconf/sysconf/defconf.tar.gz"
ETCDIR="/etc/opendomo"
LOGFILE="/var/opendomo/log/firststart.log"
DEFPKG="alsa-utils"

do_start () {
	# This a first opendomo start
	if ! test -f $DEFCONF || test -d "$SYSCONFDIR"; then
		log_warning_msg "This is a first start, configuring opendomo"

		log_action_begin_msg "Creating default configuration file"
		# Copy default config files
		manage_conf.sh copy &>$LOGFILE

		# Creating default configuration file
		cd $ETCDIR
		tar cfp $SYSCONFDIR/defconf.tar * --owner 1000 --group 1000 2>$LOGFILE && gzip -f $SYSCONFDIR/defconf.tar 2>$LOGFILE
		log_action_end_msg $?

		# Installing default packages (don't use without squashfs
#		log_action_begin_msg "Creating default configuration file"
#		apt-get install $DEFPKG
#		log_action_end_msg $?

		#TODO: Create squashfs first

		# Stop this service, don't execute anymore
		/sbin/insserv -r odfirstinit.sh  2>/dev/null

	elif ! test -d "$SYSCONFDIR"; then
			log_warning_msg "First config is not possible, odconf don't mounted"
	fi
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
        echo "Usage: odfirstinit.sh [start]" >&2
        exit 3
        ;;
esac

:
