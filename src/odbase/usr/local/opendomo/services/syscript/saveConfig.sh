#!/bin/sh
#desc:Save system configuration
#package:odbase
#type:multiple

SYSSTATE="/var/opendomo/run/Systembusy.pid"

# Wait to finish busy system state
while test -f "$SYSSTATE"; do
    sleep 1
done

sudo /usr/local/sbin/mkrootfs
/usr/local/opendomo/manageConfiguration.sh
