#!/bin/sh
#desc:Delete sequence
#type:local
#package:odcommon

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

STPATH=/etc/opendomo/states

if test -z "$1"; then
	echo "#> Delete states"
	echo "# No state specified"
	echo "#INFO: No state specified"
	exit 1
fi

for file in "$@"
do
	if test -e "$STPATH/$file"; then
		if rm -fr "$STPATH/$file"; then
			echo "#INFO: State deleted [$file]"
		else
			echo "#ERR: Cannot delete"
			exit 2
		fi
	fi
done
# Once deleted, back to the main script
/usr/local/opendomo/manageSystemStates.sh
