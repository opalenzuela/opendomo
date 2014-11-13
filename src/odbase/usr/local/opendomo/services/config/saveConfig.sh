#!/bin/sh
#desc:Save system configuration
#package:odbase
#type:multiple

#TODO Busy state is deprecated, will be replaced by json file
#SYSSTATE="/var/opendomo/run/Systembusy.pid"

# Wait to finish busy system state
#while test -f "$SYSSTATE"; do
#    sleep 1
#done

sudo mkrootfs

echo "#> Configuration saved"
echo "#INFO Your configuration was saved"
echo
echo "actions:"
echo "	reboot.sh	Reboot now"
echo
