#!/bin/sh
#desc:Save system configuration and reboot
#package:odbase
#type:multiple

SYSSTATE="/var/opendomo/run/Systembusy.pid"

# Wait to finish busy system state
while test -f "$SYSSTATE"; do
    sleep 1
done

# Send spaceless mode to opendomo apt
sudo /usr/local/sbin/mkrootfs reboot
