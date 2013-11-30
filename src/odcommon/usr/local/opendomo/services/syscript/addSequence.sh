#!/bin/sh
#desc:Add sequence
#type:local
#package:odhal

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later


# Add new sequence
echo "#> Add sequence"
echo "form:`basename $0`"
echo "	code	code	hidden	new	"
echo "	action	action	hidden	addnew"
echo "	name	Descriptive name	text"
echo "actions:"
echo "	manageSequenceSteps.sh	Add"
echo "	manageSequence.sh	Manage sequences"
echo


