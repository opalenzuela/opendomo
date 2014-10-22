#!/bin/sh
#desc:Wait
#package:odcommon
if ! test -z "$1"; then
	echo "# Waiting [$1] seconds"
	/bin/sleep $1
fi
