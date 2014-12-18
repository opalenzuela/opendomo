#!/bin/sh
#desc:First configuration wizard
#package:odbase
#type:local

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

# In the fifth step we create the user accounts and we send them an e-mail with 
# the instructions to follow. 

SYSTEMIP=`/sbin/ifconfig eth0 | grep "inet addr" | cut -f2 -d: | cut -f1 -d' '`

for email in $@; do
	username=`echo $email | cut -f1 -d@`
	/usr/local/opendomo/userModify.sh $username $email 2>/dev/null >/dev/null
done

echo "#>First configuration wizard [5/5]"
echo "form:`basename $0`	wizard"
#echo "	pres	Presentation	application	http://opalenzuela.github.io/opendomo/tour.html"
echo "#WARN System is saving configuration. Please wait"
echo "#INFO Ready to install some plugins!"
echo
echo "actions:"
echo "	goback	Previous"
echo "	managePlugins.sh	Continue"
echo
echo
/usr/local/opendomo/saveConfig.sh &  >/dev/null

exit 0
