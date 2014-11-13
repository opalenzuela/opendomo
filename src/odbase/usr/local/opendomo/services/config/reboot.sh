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
/usr/local/bin/reboot.sh >/dev/null
