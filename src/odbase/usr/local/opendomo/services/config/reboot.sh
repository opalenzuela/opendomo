#!/bin/sh
#name:Reboot
#desc:Reboot
#package:odbase
#type:local

# Copyright(c) 2015 OpenDomo Services SL. Licensed under GPL v3 or later

SYSSTATUS="/var/www/data/status.json"
echo "{\"status\":\"rebooting\"}" > $SYSSTATUS

echo "#LOADING Rebooting ... Please wait"
echo
echo "actions:"
echo
sudo shutdown -r now
