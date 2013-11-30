#!/bin/sh
#desc:First configuration wizard
#package:odcgi
#type:local

TMPCFGFILE="/tmp/wizFirstConfiguration.cfg"
. $TMPCFGFILE

URLVAL="http://cloud.opendomo.com/activate/index.php"

if test "$newpassword" != "$retype"
then
	echo "#ERR Passwords do not match"
	/usr/local/opendomo/wizFirstConfigurationStep3.sh
	exit 0
fi

echo "#LOADING Applying changes. Please wait!"
echo

echo admin "$fullname" "$email" opendomo "$2" "$3"
/usr/local/opendomo/services/config/modUser.sh admin "$fullname" "$email" opendomo "$2" "$3" > /dev/null

uid=`cat /etc/opendomo/uid` 2>/dev/null
if test -f /etc/VERSION; then
	ver=`cat /etc/VERSION` 2>/dev/null
else
	ver=`cat /mnt/system/VERSION` 2>/dev/null
fi
mail=`cat /etc/opendomo/udata/admin/email` 2>/dev/null

FULLURL="$URLVAL?UID=$uid&VER=$ver&MAIL=$mail"
wget -q -O /tmp/activation.tmp $FULLURL 2>/dev/null

#echo "#URL: $FULLURL"

# Save with old (default) user/pass
/usr/local/opendomo/saveSystemConfiguration.sh admin opendomo > /dev/null
echo "#> Configuration finished"
echo "#INFO Your configuration was saved"
echo "# You can now start using your OpenDomo"
echo
/usr/bin/list.sh /
if test -x /usr/local/opendomo/services/syscript/eh_odalerts.sh
then
	/usr/local/opendomo/services/syscript/eh_odalerts.sh notice odcommon "Your OpenDomo was successfully registered"
fi
echo

