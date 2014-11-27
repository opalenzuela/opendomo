#!/bin/sh
#desc:First configuration wizard
#package:odbase
#type:local

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

# In the second step, we save the information obtained from the first one
# (e-mail and language) and we display the license in the selected language
# or English if it's not available.

TMPCFGFILE="/var/opendomo/tmp/wizFirstConfiguration.cfg"
touch $TMPCFGFILE
. $TMPCFGFILE

if test "$stepfrom" = "1"; then
	email="$2"
	language="$3"
	if test -z "$2" || test -z "$3"
	then
		echo "#ERR: Missing values"
		/usr/local/opendomo/services/syscript/wizFirstConfigurationStep1.sh
		exit 0
	fi
	echo "email=$email"	  > $TMPCFGFILE
	echo "language=$language" >> $TMPCFGFILE
	echo "$language" > /etc/opendomo/i18n/lang
	echo "#LOADING Updating language files"
	echo

	#FIXME This cannot be hardcoded
	# Generating locales
	if [ "$language" = "es" ]; then
		echo						>>/home/admin/.profile
		echo "# Configure locales"			>>/home/admin/.profile
		echo "export LANG=es_ES.UTF-8" 		>>/home/admin/.profile
		echo "export LC_ADDRESS=es_ES.UTF-8"		>>/home/admin/.profile
		echo "export LC_MEASUREMENT=es_ES.UTF-8"	>>/home/admin/.profile
		echo "export LC_NAME=es_ES.UTF-8"		>>/home/admin/.profile
	elif [ "$language" = "en" ]; then
		echo						>>/home/admin/.profile
		echo "# Configure locales"			>>/home/admin/.profile
		echo "export LANG=en_US.UTF-8"		>>/home/admin/.profile
		echo "export LC_ADDRESS=en_US.UTF-8"		>>/home/admin/.profile
		echo "export LC_MEASUREMENT=en_US.UTF-8"	>>/home/admin/.profile
		echo "export LC_NAME=en_US.UTF-8"		>>/home/admin/.profile
	fi

	/usr/local/opendomo/services/syscript/updateLanguageFiles.sh > /dev/null
fi

echo "#> First configuration wizard [2/5]"
echo "form:`basename $0`	wizard"
echo "	stepfrom	step	hidden	2"
test -f /var/www/$language.license.html || $language="en"
echo "	license	License	application	/$language.license.html"
echo "actions:"
echo "	goback	Previous"
echo "	wizFirstConfigurationStep3.sh	Accept"
echo
