#!/bin/sh
#desc:Restore default system
#package:odcommon
#type:multiple

echo "#INFO Restore system to default status ..."
sudo /usr/local/sbin/mkrootfs restore >/dev/null 2>/dev/null
echo "#WARN Rebooting system ..."
reboot.sh
