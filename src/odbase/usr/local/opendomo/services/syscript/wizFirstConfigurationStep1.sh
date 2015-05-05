#!/bin/sh
#desc:First configuration wizard
#package:odbase
#type:local

# Copyright(c) 2015 OpenDomo Services SL. Licensed under GPL v3 or later

# This is the first configuration wizard. After installing the system, the
# admin user must go through this wizard before being able to use it.
# In the first step, we ask for admin's e-mail address and the language.

CFGPATH="/etc/opendomo"
TMPCFGFILE="/var/opendomo/tmp/wizFirstConfiguration.cfg"
ADMCFGFILE="$CFGPATH/udata/admin.info"

fullname=`grep ^FULLNAME= $ADMCFGFILE | sed 's/\"//g' | cut -f2 -d= `
email=`grep ^EMAIL= $ADMCFGFILE | sed 's/\"//g' | cut -f2 -d= `
language="es"

wget -q http://cloud.opendomo.com/babel/trans.php?lang=all -O - | grep -v  '<' > $CFGPATH/langfiles/available 
LANGUAGES=`cat $CFGPATH/langfiles/available `

touch $TMPCFGFILE
. $TMPCFGFILE

if test -f /boot/images/rootfs.img; then
	echo "#> First configuration wizard [1/5]"
	echo "form:`basename $0`	wizard"
	echo "	stepfrom	step	hidden	1"
	echo "	sep	Enter a valid administrator e-mail and language	separator"
	echo "	email	Administrator e-mail address	email	$email"
	echo "	language	Language	list[$LANGUAGES]	$language"
	echo "actions:"
	echo "	wizFirstConfigurationStep2.sh	Next"
else
	echo "# OpenDomoOS cannot find a proper unit to save configuration"
	echo "# Find how to solve this problem clicking the button 'More information'"
	exit 1
fi
echo
