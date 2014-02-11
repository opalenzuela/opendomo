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
echo "actions:"
echo "	wizFirstConfigurationStep4.sh	Finish"
echo "	wizFirstConfigurationStep2.sh	Previous"
echo

