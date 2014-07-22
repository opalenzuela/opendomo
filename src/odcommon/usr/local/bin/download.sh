#!/bin/sh
#desc: download the specified file

if test -z "$1"; then
	echo "usage: $0 URL OUTPUT"
	echo
	exit 1
fi

if test -z "$2"; then
	FNAME=`basename $1`
else
	FNAME="$2"
fi

if wget --no-check-certificate -q $1 -O $FNAME
then
	exit 0
else
	exit 1
fi
