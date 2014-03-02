#!/bin/sh
#desc:First configuration wizard
#package:odcgi
#type:local

TMPCFGFILE="/var/opendomo/tmp/wizFirstConfiguration.cfg"
ADMCFGFILE="/etc/opendomo/udata/admin.info"
fullname=`grep ^FULLNAME= $ADMCFGFILE | sed 's/\"//g' | cut -f2 -d= `
email=`grep ^EMAIL= $ADMCFGFILE | sed 's/\"//g' | cut -f2 -d= `
language="es"
touch $TMPCFGFILE
. $TMPCFGFILE

if test -e /mnt/odconf ; then
	echo "#> First configuration wizard [1/3]"
	echo "form:`basename $0`	wizard"
	echo "	stepfrom	step	hidden	1"
	echo "	sep	Enter your name and e-mail	separator"
	echo "	fullname	Full user name	text:[a-zA-Z ]*	$fullname"
	echo "	email	e-mail address	email	$email"
	echo "	language	Language	list[en,es]	$language"
	echo "#TIP(fullname) OpenDomo will address you with this name"
	echo "#TIP(email) The e-mail address is used for sending notifications"
	echo "actions:"
	echo "	wizFirstConfigurationStep2.sh	Next"
else
	echo "# OpenDomo cannot find a proper unit to save configuration"
	echo "# Find how to solve this problem clicking the button 'More information'"
	exit 1
fi
echo
