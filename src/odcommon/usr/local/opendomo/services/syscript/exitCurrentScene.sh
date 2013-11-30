#!/bin/sh
#desc:Exit current scene
#type:local
#package:odhal

if test -f /tmp/exitscene.tmp
then
	echo "#INFO Exiting previous scene"
	# Execute the recovery script created by setScene
	/bin/sh /tmp/exitscene.tmp
	rm /tmp/exitscene.tmp
	rm /tmp/lastscene.tmp
	# Display the available scenes
	/usr/local/opendomo/setScene.sh
else
	echo "#ERR Information about previous state not found"
	exit 1
fi
