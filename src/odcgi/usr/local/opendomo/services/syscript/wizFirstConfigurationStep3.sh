#!/bin/sh
#desc:First configuration wizard
#package:odcgi
#type:local

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
echo "	latitude	latitude	text	"
echo "	longitude	longitude	text	"
echo "	timezone	Timezone	text	"
echo "	timezoneid	Timezone ID	text	"
echo "	city	City	text	"
echo "	address	Address	text	"
echo "actions:"
echo "	goback	Previous"
echo "	wizFirstConfigurationStep4.sh	Finish"
echo
