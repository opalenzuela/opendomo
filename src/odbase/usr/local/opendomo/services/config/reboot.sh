#!/bin/sh
#name:Reboot
#desc:Reboot
#package:odbase
#type:local

# TODO: Json system busy implementation

echo "# Rebooting ... Please wait"
echo
echo "actions:"
echo
sudo shutdown -r now
