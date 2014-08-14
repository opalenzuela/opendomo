#!/bin/sh
#desc:Save system configuration
#package:odcommon
#type:multiple

SYSMODE="/var/opendomo/run/systembusy"

# Check system status
if test -f $SYSMODE; then
    echo "#ERROR Configuration can't be saved, system busy"
else
    sudo /usr/local/sbin/mkrootfs
fi
/usr/local/opendomo/manageConfiguration.sh
