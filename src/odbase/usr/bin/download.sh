#!/bin/sh
#desc: download the specified file

if test -z "$1"; then
	echo "usage: $0 URL"
	echo
	exit 1
fi

FNAME=`basename $1`
if wget --no-check-certificate -q $1
then
	exit 0
else
	exit 1
fi
