#!/bin/sh
#type:restricted
#desc:Register output states
#package:odhal

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

if ! test -f /usr/local/opendomo/eventhandlers/processRules.sh
then
	return 1
fi

LOGFILE="/var/opendomo/log/odhal-st.h`date +%H`"
tstamp=`date +%H:%M:%S`

/usr/local/opendomo/eventhandlers/processRules.sh >/dev/null

touch $LOGFILE
chmod o+w $LOGFILE
echo "$tstamp $3" | sed -e 's/set output //' -e 's/[^a-zA-Z0-9 :]//g' -e 's/ to /=/'>> $LOGFILE
