#!/bin/sh
#desc:Add zones
#type:local
#package:odcommon

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later


echo
echo "#> Add zone"
echo "form:`basename $0`"
echo "	action	action	hidden	add"
echo "	zone	Code	hidden	"
echo "	desc	Description	text"
echo "	coord	Coordinates	text	10,10,100,200"
echo "action:"
echo "	manageZones.sh	Add"
echo 
echo "#> Preview"
echo "list:displayZone.sh	map"
echo "	demozone	Zone	zone"
echo
