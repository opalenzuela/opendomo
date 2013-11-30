#!/bin/sh
CRONFILE=/etc/crontab
CRONDIR=/etc/crond.d/admin
# If the crondir is newer than the cronfile...
if test $CRONDIR -nt $CRONFILE; then
	/bin/logevent notice crond "File updated" $CRONDIR
	/etc/init.d/crond reload
fi
