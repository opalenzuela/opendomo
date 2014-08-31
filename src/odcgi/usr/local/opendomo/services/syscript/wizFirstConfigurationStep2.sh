#!/bin/sh
#desc:First configuration wizard
#package:odcgi
#type:local

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

TMPCFGFILE="/var/opendomo/tmp/wizFirstConfiguration.cfg"
touch $TMPCFGFILE
. $TMPCFGFILE

if test "$stepfrom" = "1"; then
	fullname="$2"
	email="$3"
	language="$4"
	if test -z "$3" || test -z "$2"
	then
		echo "#ERR: Missing values"
		/usr/local/opendomo/services/syscript/wizFirstConfigurationStep1.sh
		exit 0
	fi
	echo "fullname=\"$fullname\""  > $TMPCFGFILE
	echo "email=$email"	  >> $TMPCFGFILE
	echo "language=$language" >> $TMPCFGFILE
	echo "$language" > /etc/opendomo/i18n/lang
	echo "#LOADING Updating language files"
	echo

	# Generating locales
	if [ "$language" = "es" ]; then
		echo						>>/home/admin/.profile
		echo "# Configure locales"			>>/home/admin/.profile
		echo "export LANG="es_ES.UTF-8"" 		>>/home/admin/.profile
		echo "export LC_ADDRESS="es_ES.UTF-8""		>>/home/admin/.profile
		echo "export LC_MEASUREMENT="es_ES.UTF-8""	>>/home/admin/.profile
		echo "export LC_NAME="es_ES.UTF-8""		>>/home/admin/.profile
	elif [ "$language" = "en" ]; then
		echo						>>/home/admin/.profile
		echo "# Configure locales"			>>/home/admin/.profile
		echo "export LANG="en_US.UTF-8""		>>/home/admin/.profile
		echo "export LC_ADDRESS="en_US.UTF-8""		>>/home/admin/.profile
		echo "export LC_MEASUREMENT="en_US.UTF-8""	>>/home/admin/.profile
		echo "export LC_NAME="en_US.UTF-8""		>>/home/admin/.profile
	fi

	/usr/local/opendomo/services/syscript/updateLanguageFiles.sh > /dev/null
fi

echo "#> First configuration wizard [2/3]"
echo "form:`basename $0`	wizard"
echo "	stepfrom	step	hidden	2"
echo "	license	License	application	/$language.license.html"
echo "actions:"
echo "	goback	Previous"
echo "	wizFirstConfigurationStep3.sh	Accept"
echo
