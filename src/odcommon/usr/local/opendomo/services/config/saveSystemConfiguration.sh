#!/bin/sh
#desc:Save configuration
#package:odcommon
#type:multiple

TMPFILE="/var/opendomo/tmp/savestatus"
CONFEXE="/usr/local/bin/manage_conf.sh"
IMGEXE="/usr/local/sbin/mkrootfs"

# Creating default temp file
if ! test -f $TMPFILE; then
	echo "off" > $TMPFILE
fi
SAVEPLUGINS=`cat $TMPFILE`

# Select option
if [ "$1" = "plugins" ]; then
	echo "$2" > $TMPFILE
elif [ "$1" = "save" ]; then
	if [ $SAVEPLUGINS = "yes" ]; then
		echo "#INF Configuration and plugins saved"
		$CONFEXE save
		sudo $IMGEXE custom
	else
		echo "#INF Configuration saved"
		$CONFEXE save
	fi
else
	# No options, see interface
	echo "#> Saving data"
        echo "form:`basename $0`"
	echo "	plugins	Save plugins	subcommand[on,off]	$SAVEPLUGINS"
	echo "	save	Save data	hidden	save"
	echo
fi
