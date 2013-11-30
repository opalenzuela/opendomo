#!/bin/sh
#desc:Delete sequence
#type:local
#package:odcommon

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

SEQPATH="/etc/opendomo/sequences"

if test -z "$1"; then
	echo "#> Delete states"
	echo "# No sequence specified"
	echo "#INFO: No sequence specified"
	exit 1
fi

for file in "$@"
do
	if test -f "$SEQPATH/$file"; then
		if rm -f "$SEQPATH/$file"; then
			echo "#INFO: Sequence deleted [$file]"
		else
			echo "#ERR: Cannot delete"
			exit 2
		fi
	fi
done
# Once deleted, back to the main script
/usr/local/opendomo/manageSequence.sh
