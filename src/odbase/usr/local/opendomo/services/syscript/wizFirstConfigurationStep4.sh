#!/bin/sh
#desc:First configuration wizard
#package:odbase
#type:local

# Copyright(c) 2015 OpenDomo Services SL. Licensed under GPL v3 or later

# The fourth step will store the collected information in its final configuration 
# file, activate the system into the Cloud (so we can use messaging) and we will
# also ask for some e-mails in order to create user accounts.

GEOLOCFILE="/etc/opendomo/geo.conf"
TMPCFGFILE="/var/opendomo/tmp/wizFirstConfiguration.cfg"
. $TMPCFGFILE
URLVAL="http://cloud.opendomo.com/activate/index.php"
UIDFILE="/etc/opendomo/uid"
TZDATA="/usr/share/zoneinfo"
VERSION=`apt-cache show odbase | grep Version| cut -f2 -d' '`

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

# If empty, use default: 41.384230, 2.173659
if test -z "$latitude"; then
	timezoneid="Europe/Madrid"
	latitude="41.384230"
	longitude="2.173659"
	timezone="1"
	city="Barcelona"
	address=""
fi

# Save geolocation and configure timezone
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
sudo usermod -c "System administrator <$email>" admin
echo -e "$newpassword\n$newpassword" | (sudo passwd admin) 2>/dev/null

# Activate OpenDomoOS system into the cloud
FULLURL="$URLVAL?UID=$uid&VER=$VERSION&MAIL=$email"
wget -q -O /var/opendomo/tmp/activation.tmp $FULLURL 2>/dev/null

echo "#>First configuration wizard [4/5]"
echo "form:`basename $0`	wizard"
echo "#INFO Is dangerous to use OpenDomoOS as administrator, so let's create some user accounts."
echo "	user1	User 1	readonly	$email"
echo "	user2	User 2	email"
echo "	user3	User 3	email"
echo "#TIP Don't worry about the other information. Each user will be prompted for it later."
#echo "#INFO System successfully configured, ready to install plugins"
echo "actions:"
echo "	goback	Previous"
echo "	wizFirstConfigurationStep5.sh	Next"
echo
