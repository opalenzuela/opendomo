#!/bin/sh
#desc:First configuration wizard
#package:odcgi
#type:local

GEOLOCFILE="/etc/opendomo/geo.conf"
TMPCFGFILE="/var/opendomo/tmp/wizFirstConfiguration.cfg"
. $TMPCFGFILE
URLVAL="http://cloud.opendomo.com/activate/index.php"

# Checking password
if test "$newpassword" != "$retype"
then
	echo "#ERR Passwords do not match"
	/usr/local/opendomo/wizFirstConfigurationStep3.sh
	exit 0
fi

# Save geolocation and configure timezone
echo "latitude=$latitude"	 > $GEOLOCFILE
echo "longitude=$longitude"	>> $GEOLOCFILE
echo "timezone=$timezone"       >> $GEOLOCFILE
echo "timezoneid=$timezoneid"   >> $GEOLOCFILE
echo "city=$city"               >> $GEOLOCFILE
echo "address=$address"         >> $GEOLOCFILE

echo "$timezone" > /etc/timezone
LC_ALL=C LANGUAGE=C LANG=C DEBIAN_FRONTEND=noninteractive sudo dpkg-reconfigure tzdata &>/dev/null

# Saving new user data
sudo manageusers.sh mod admin "$fullname" "$email" "$newpassword" >/dev/null 2>/dev/null

# Activate
FULLURL="$URLVAL?UID=$uid&VER=$ver&MAIL=$mail"
wget -q -O /var/opendomo/tmp/activation.tmp $FULLURL 2>/dev/null

# Save system and reboot
saveConfigReboot.sh
