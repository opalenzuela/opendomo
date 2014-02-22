#!/bin/sh
#desc:Save system configuration
#package:odcommon
#type:multiple

echo -n "#INFO Save system configuration (please wait) ..."
sudo /usr/local/sbin/mkrootfs custom >/dev/null 2>/dev/null
echo "   (done)"

/usr/local/opendomo/manageConfiguration.sh
