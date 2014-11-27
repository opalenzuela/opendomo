#!/bin/sh
#desc:First configuration wizard
#package:odbase
#type:local

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

# In the third step we ask for a new password for the admin, and we use
# geopositioning features to locate the facility. This will be useful in 
# order to determine timezone, but also for other features that we will 
# use in the future (for instance, the weather station).

TMPCFGFILE="/var/opendomo/tmp/wizFirstConfiguration.cfg"
touch $TMPCFGFILE
. $TMPCFGFILE

echo "#> First configuration wizard [3/5]"
echo "form:`basename $0`	wizard"
echo "#INFO You must enter a new administrator password"
echo "	stepfrom	step	hidden	3"
echo "	newpassword	New password	password"
echo "	retype	Retype password	password"
echo "	sep	Adjustments	separator"
echo "	latitude	latitude	hidden	"
echo "	longitude	longitude	hidden	"
echo "	timezone	timezone offset	hidden	"
echo "	timezoneid	Timezone 	readonly	"
echo "	city	City	hidden	"
echo "	address	Address	hidden	"
echo "#TIP OpenDomoOS will try to find your position to adjust the settings, but don't worry, this information will never leave your device!"
echo "actions:"
echo "	goback	Previous"
echo "	wizFirstConfigurationStep4.sh	Next"
echo
