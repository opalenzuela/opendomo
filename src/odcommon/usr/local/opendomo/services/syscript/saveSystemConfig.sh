#!/bin/sh
#desc:Save system configuration
#package:odcommon
#type:multiple

SYSUPDATEPID="/var/opendomo/run/updateSystem.pid"
INSPLUGINPID="/var/opendomo/tmp/installplugin.pid"
PIDFILE="/var/opendomo/run/saveSystemConf.pid"

# Check process
if test -f $PIDFILE; then
    echo "#ERRO Save config is already running"
    exit 1
fi

# Creating PID
touch $PIDFILE

# Waiting system update or plugins instalation
while test -f $SYSUPDATEPID || test -f $INSPLUGINPID; do
    sleep 1
done

echo "#LOADING Save system configuration (please wait) ..."
echo
if
sudo /usr/local/sbin/mkrootfs custom >/dev/null 2>/dev/null
then
    echo "#INFO Configuration saved"
else
    echo "#ERRO Configuration can be saved"
fi
rm $PIDFILE

/usr/local/opendomo/manageConfiguration.sh
