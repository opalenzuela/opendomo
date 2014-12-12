#!/bin/sh
#desc:Send mail notification
#type:redundant
#package:odbase

### Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

MAIL="$1"
URL="http://cloud.opendomo.com/alert/index.php"
TMPFILE="/var/opendomo/tmp/$$.tmp"

TEXT=`/usr/local/bin/i18n.sh "$2" | sed 's/ /+/g'`
AUID=`cat /etc/opendomo/uid`
FURL="$URL?UID=$AUID&MAIL=$EMAIL&TEXT=$TEXT"

rm -fr $TMPFILE 2>/dev/null
wget -q $FURL -O $TMPFILE 2>/dev/null
if grep -q ERROR $TMPFILE; then
        exit 1
else
        exit 0
fi
