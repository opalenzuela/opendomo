#!/bin/sh
#desc:First configuration wizard
#package:odcgi
#type:local

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

# Saving new user data
sudo manageusers.sh mod admin "$fullname" "$email" "$newpassword" >/dev/null 2>/dev/null

# Activate
FULLURL="$URLVAL?UID=$uid&VER=$ver&MAIL=$mail"
wget -q -O /var/opendomo/tmp/activation.tmp $FULLURL 2>/dev/null

# Updating system
echo "#LOADING Updating system. Please wait!"
echo
updateSystem.sh >/dev/null 2>/dev/null

# Saving configuration
echo "#LOADING Saving changes. Please wait!"
echo
saveSystemConfig.sh >/dev/null 2>/dev/null

echo "#> Configuration finished"
echo "#INFO Your configuration was saved"
echo "#INFO You can now start using your OpenDomo"
echo
/usr/bin/list.sh /
echo
