#!/bin/sh
#desc:Send mail notification to admin
#type:redundant
#package:odbase

### Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later


LEVEL="$1"
MODULE="$2"
URL="http://cloud.opendomo.com/alert/index.php"
TMPFILE="/var/opendomo/tmp/msgconfirmation.tmp"

TEXT=`/usr/local/bin/i18n.sh "$3" | sed 's/ /+/g'`
AUID=`cat /etc/opendomo/uid`
source /etc/opendomo/udata/admin.info 
FURL="$URL?UID=$AUID&MAIL=$EMAIL&TEXT=$TEXT&LEVEL=$LEVEL&MODULE=$MODULE"

rm -fr $TMPFILE 2>/dev/null
wget -q $FURL -O $TMPFILE 2>/dev/null
if grep -q ERROR $TMPFILE; then
        exit 1
else
        exit 0
fi
