#!/bin/sh
#desc:Send mail notification
#type:redundant
#package:odbase

# Copyright(c) 2015 OpenDomo Services SL. Licensed under GPL v3 or later

if test -z "$1"; then
	echo "usage:$0 e-mail [Subject] Message [link]"
	exit 0
fi

EMAIL="$1"
URL="http://cloud.opendomo.com/alert/index.php"
TMPFILE="/var/opendomo/tmp/$$.tmp"

TEXT=`i18n.sh "$2" | sed 's/ /+/g'`
if ! test -z "$3"; then
	SUBJECT="$TEXT"
	TEXT=`i18n.sh "$3" | sed 's/ /+/g'`
fi
if ! test -z "$4"; then
	LINK="$4"
fi



AUID=`cat /etc/opendomo/uid`
FURL="$URL?UID=$AUID&MAIL=$EMAIL&TEXT=$TEXT&SUBJECT=$SUBJECT&LINK=$LINK"

rm -fr $TMPFILE 2>/dev/null
wget -q $FURL -O $TMPFILE 2>/dev/null
if grep -q ERROR $TMPFILE; then
        exit 1
else
        exit 0
fi
