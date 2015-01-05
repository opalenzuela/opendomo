#!/bin/sh
#desc:Save system configuration
#package:odbase
#type:multiple

PIDFILE="/var/opendomo/run/mkrootfs.pid"

message() {
    echo "#LOADING Save system configuration (please wait) ..."
    echo
    while  pstree | grep mkrootfs >/dev/null; do
        sleep 1
    done
    echo "#INFO Your configuration was saved"
    echo
    echo "actions:"
    echo "	reboot.sh	Reboot now"
    echo
}

if   [ `cat /var/www/data/status.json | cut -f2 -d: | cut -f1 -d}` = '"busy"' ] && pstree | grep -c mkrootfs >/dev/null; then
    # Configuration is already saving
    message

elif [ `cat /var/www/data/status.json | cut -f2 -d: | cut -f1 -d}` = '"busy"' ]; then
    # System is busy by other process
    echo
    echo "#WARN Configuration can be saved yet, system is busy"
    echo

else
    # Saving configuration
    sudo mkrootfs &
    sleep 1
    message
fi
