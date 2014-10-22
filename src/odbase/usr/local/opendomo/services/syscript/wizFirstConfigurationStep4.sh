#!/bin/sh
#desc:First configuration wizard
#package:odcgi
#type:local

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

GEOLOCFILE="/etc/opendomo/geo.conf"
TMPCFGFILE="/var/opendomo/tmp/wizFirstConfiguration.cfg"
. $TMPCFGFILE
URLVAL="http://cloud.opendomo.com/activate/index.php"
UIDFILE="/etc/opendomo/uid"
TZDATA="/usr/share/zoneinfo"
source /etc/os-release

# Checking password
if test "$newpassword" != "$retype"
then
	echo "#ERR Passwords do not match"
	/usr/local/opendomo/wizFirstConfigurationStep3.sh
	exit 0
fi

# Generate UID only in the last step
MACADDRESS=`/sbin/ifconfig eth0 | grep HWaddr | cut -c 39-55 | sed -e 's/://g'` 
echo "$email `date` $MACADDRESS" | sha256sum | cut -f1 -d' ' > $UIDFILE 
uid=`cat  $UIDFILE `

# Save geolocation and configure timezone
if test -z "$latitude"; then
	timezoneid="Europe/Madrid"
	latitide="41"
	longitude="2"
	timezone="1"
	city="Madrid"
	address=""
fi

echo "latitude='$latitude'"	 > $GEOLOCFILE
echo "longitude='$longitude'"	>> $GEOLOCFILE
echo "timezone='$timezone'"     >> $GEOLOCFILE
echo "timezoneid='$timezoneid'" | sed 's/+/ /g' >> $GEOLOCFILE
echo "city='$city'"             | sed 's/+/ /g' >> $GEOLOCFILE
echo "address='$address'"       | sed 's/+/ /g' >> $GEOLOCFILE

# Setting timezone
echo "$timezoneid" > /etc/timezone
rm /etc/opendomo/system/localtime
cp -rp "$TZDATA/$timezoneid" "/etc/opendomo/system/localtime"

# Saving new user data
sudo manageusers.sh mod admin "$fullname" "$email" "$newpassword" >/dev/null 2>/dev/null

# Activate
FULLURL="$URLVAL?UID=$uid&VER=$VERSION&MAIL=$email"
wget -q -O /var/opendomo/tmp/activation.tmp $FULLURL 2>/dev/null

# Save system and reboot
#saveConfigReboot.sh

echo "#>System configured"
echo "list:`basename $0`"
echo "#INFO System successfully configured, ready to install plugins"
echo "actions:"
echo "	managePlugins.sh	Continue"
echo
