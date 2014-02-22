#!/bin/sh
#desc:Load default configuration
#package:odcommon
#type:local

echo -n "#INFO Load default configuration files ..."
sudo manage_conf.sh restore 	>/dev/null 2>/deV/null
rm -r /etc/opendomo/* 		>/dev/null 2>/dev/null
sudo manage_conf.sh copy 	>/dev/null 2>/dev/null
echo "   (done)"

/usr/local/opendomo/manageConfiguration.sh
