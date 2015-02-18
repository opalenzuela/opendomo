#!/bin/sh
#desc This script will load all the required JS in a single HTTP query

# Copyright(c) 2015 OpenDomo Services SL. Licensed under GPL v3 or later

JSFILE="/var/opendomo/tmp/javascript.tmp"

if ! test -z "$HTTP_IF_MODIFIED_SINCE" && test -f $JSFILE; then
	echo "HTTP/1.1 304 Not Modified"
	echo
	exit
else
	if ! test -f $JSFILE; then
		cd /var/www/scripts/common/
		#echo "/*" > $JSFILE
		#env >> $JSFILE
		#echo "*/" >> $JSFILE
		for file in *.js
		do
			echo "/* File $file*/" >> $JSFILE
			cat $file >> $JSFILE
			echo >> $JSFILE
		done
		#TODO Compress/cleanup file
	fi
	FILESIZE=`wc -c $JSFILE |cut -f1 -d' '`
	ETAG=`md5sum $JSFILE | cut -f1 -d' '`
	DATE=`date -R --date='last Fri'`
	EXPIRE=`date -R --date='next Fri'`
	echo "Vary: Accept-Encoding"
	echo "Cache-Control: max-age=3600, must-revalidate"
	echo "Date: $DATE"
	echo "Expires: $EXPIRE"
	echo "Last-Modified: $DATE"
	#echo "ETag: $ETAG"
	echo "Content-Length: $FILESIZE"
	echo "Content-Type: text/javascript"
	echo
	cat $JSFILE
fi
