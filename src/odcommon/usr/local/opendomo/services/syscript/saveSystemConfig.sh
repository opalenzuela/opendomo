#!/bin/sh
#desc:Save system configuration
#package:odcommon
#type:multiple

SYSUPDATEPID="/var/opendomo/run/updateSystem.pid"
PIDFILE="/var/opendomo/run/saveSystemConf.pid"

# Check process
if test -f $PIDFILE; then
    echo "#ERRO Save config is already running"
    exit 1
fi

# Creating PID
touch $PIDFILE

# Waiting system update in first configuration script
while test -f $SYSUPDATEPID; do
    sleep 1
done

echo "#LOADING Save system configuration (please wait) ..."
echo
sudo /usr/local/sbin/mkrootfs custom >/dev/null 2>/dev/null
rm $PIDFILE

/usr/local/opendomo/manageConfiguration.sh
