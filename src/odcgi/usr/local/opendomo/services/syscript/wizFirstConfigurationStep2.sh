#!/bin/sh
#desc:First configuration wizard
#package:odcgi
#type:local

TMPCFGFILE="/var/opendomo/tmp/wizFirstConfiguration.cfg"
touch $TMPCFGFILE
. $TMPCFGFILE

if test "$stepfrom" = "1"; then
	fullname="$2"
	email="$3"
	language="$4"
	echo "fullname=$fullname"  > $TMPCFGFILE
	echo "email=$email"	  >> $TMPCFGFILE
	echo "language=$language" >> $TMPCFGFILE
	echo "$language" > /etc/opendomo/i18n/lang
	echo "#LOADING Updating language files"
	echo
	/usr/local/opendomo/services/syscript/updateLanguageFiles.sh > /dev/null
fi

echo "#> First configuration wizard [2/3]"
echo "form:`basename $0`	wizard"
echo "	stepfrom	step	hidden	2"
echo "	license	License	application	/$language.license.html"
echo "actions:"
echo "	wizFirstConfigurationStep3.sh	Accept"
echo "	wizFirstConfigurationStep1.sh	Previous"
echo
