#!/bin/sh
#desc: download the specified file

if test -z "$1"; then
	echo "usage: $0 URL"
	echo
	exit 1
fi

FNAME=`basename $1`
if wget -q $1 -O $FNAME
then
	exit 0
else
	exit 1
fi