#!/bin/sh
#desc:First configuration wizard
#package:odbase
#type:local

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

TMPCFGFILE="/var/opendomo/tmp/wizFirstConfiguration.cfg"
touch $TMPCFGFILE
. $TMPCFGFILE

echo "#> First configuration wizard [2/3]"
echo "form:`basename $0`	wizard"
echo "	stepfrom	step	hidden	3"
echo "	sep	You must enter a new administrator password	separator"
echo "	newpassword	New password	password"
echo "	retype	Retype password	password"
echo "	sep	Adjustments	separator"
echo "	latitude	latitude	hidden	"
echo "	longitude	longitude	hidden	"
echo "	timezone	timezone offset	hidden	"
echo "	timezoneid	Timezone 	readonly	"
echo "	city	City	hidden	"
echo "	address	Address	hidden	"
echo "actions:"
echo "	goback	Previous"
echo "	wizFirstConfigurationStep4.sh	Finish"
echo
