#!/bin/sh
#desc:Save system configuration
#package:odcommon
#type:multiple

SYSUPDATEPID="/var/opendomo/run/updateSystem.pid"
PIDFILE="/var/opendomo/run/saveSystemConf.pid"

# creating PID
touch $PIDFILE

# Waiting system update in first configuration script
while -f $SYSUPDATEPID; do
    sleep 1
done

echo -n "#INFO Save system configuration (please wait) ..."
sudo /usr/local/sbin/mkrootfs custom >/dev/null 2>/dev/null
echo "   (done)"
rm $PIDFILE

/usr/local/opendomo/manageConfiguration.sh
