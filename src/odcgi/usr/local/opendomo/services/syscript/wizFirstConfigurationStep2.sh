#!/bin/sh
#desc:First configuration wizard
#package:odcgi
#type:local

TMPCFGFILE="/var/opendomo/tmp/wizFirstConfiguration.cfg"
UIDFILE="/etc/opendomo/uid"
touch $TMPCFGFILE
. $TMPCFGFILE

if test "$stepfrom" = "1"; then
	fullname="$2"
	email="$3"
	language="$4"
	echo "fullname=\"$fullname\""  > $TMPCFGFILE
	echo "email=$email"	  >> $TMPCFGFILE
	echo "language=$language" >> $TMPCFGFILE
	echo "$language" > /etc/opendomo/i18n/lang
	echo "#LOADING Updating language files"
	MACADDRESS=`/sbin/ifconfig eth0 | grep HWaddr | cut -c 39-55 | sed -e 's/://g'` 
	echo "$email `date` $MACADDRESS" | sha256sum | cut -f1 -d' ' > $UIDFILE 
	echo

	# Generating user config
	cp /etc/skel/.* /home/admin/

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
echo "	wizFirstConfigurationStep3.sh	Accept"
echo "	wizFirstConfigurationStep1.sh	Previous"
echo
