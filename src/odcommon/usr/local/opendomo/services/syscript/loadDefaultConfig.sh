#!/bin/sh
#desc:Load default configuration
#package:odcommon
#type:local

echo "#LOADING Load default configuration files ..."
sudo manage_conf.sh restore 	>/dev/null 2>/deV/null
rm -r /etc/opendomo/* 		>/dev/null 2>/dev/null
sudo manage_conf.sh copy 	>/dev/null 2>/dev/null

/usr/local/opendomo/manageConfiguration.sh
