#!/bin/sh
#desc:Add sequence
#type:local
#package:odcommon

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later


# Add new state
echo "#> Add new state"
echo "form:`basename $0`"
echo "	action	action	hidden	add"
echo "	key	Key	text:[a-z0-9]+	$2"
echo "	name	Name	text"	$3
echo "actions:"
echo "	manageSystemStates.sh	Add"
echo


