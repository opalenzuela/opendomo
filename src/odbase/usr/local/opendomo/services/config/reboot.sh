#!/bin/sh
#name:Reboot
#desc:Reboot
#package:odbase
#type:local

SYSSTATUS="/var/www/data/status.json"
echo "{\"status\":\"rebooting\"}" > $SYSSTATUS

echo "#LOADING Rebooting ... Please wait"
echo
echo "actions:"
echo
sudo shutdown -r now
