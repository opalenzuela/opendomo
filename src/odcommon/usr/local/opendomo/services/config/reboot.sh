#!/bin/sh
#name:Reboot
#desc:Reboot
#package:odcommon
#type:local

SYSUPDATPID="/var/opendomo/run/updateSystem.pid"
SAVECONFPID="/var/opendomo/run/saveSystemConf.pid"

if test -f $SYSUPDATPID; then
	echo "#ERRO System update is running, system can be reboot"
	echo
elif test -f $SAVECONFPID; then
	echo "#ERRO Save config is running, system can be reboot"
	echo
else
	echo "# Rebooting ... Please wait"
	echo
	echo "actions:"
	echo
	/usr/local/bin/reboot.sh >/dev/null
fi
