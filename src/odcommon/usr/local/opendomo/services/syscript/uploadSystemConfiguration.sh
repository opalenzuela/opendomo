#!/bin/sh
#desc:Upload configuration
#package:odcommon
#type:multiple

CFGFILE="/mnt/odconf/sysconf/sysconf.tar"
LOG="/var/log/savecfg.log"
ERR="/var/log/savecfg.err"
ENC="/tmp/cfgbackup.enc"

if test -f "$ENC"
then
	echo "#LOADING Uploading configuration file"
	echo
	curl -F file=$ENC http://cloud.opendomo.com/upgrade/
	echo "#INFO File uploaded"
else
	echo "#ERR File not found"
	exit 1
fi
echo
