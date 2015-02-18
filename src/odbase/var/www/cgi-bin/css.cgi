#!/bin/sh
#desc This script selects the right CSS file

# Copyright(c) 2015 OpenDomo Services SL. Licensed under GPL v3 or later

CSSFILE="/var/opendomo/tmp/stylesheets.tmp"

if ! test -z "$HTTP_IF_MODIFIED_SINCE" && test -f $CSSFILE; then
	echo "HTTP/1.1 304 Not Modified"
	echo
	exit
else
	if ! test -f $CSSFILE; then
		STYLE=`cat /etc/opendomo/cgi_style`
		test -z "$STYLE" && STYLE=default
		echo " /* STYLE $STYLE */" >> $CSSFILE
		cat /var/www/themes/$STYLE/main.css >> $CSSFILE
		
		SKIN=`cat /etc/opendomo/cgi_skin` 
		test -z "$SKIN" && SKIN=default
		echo " /* SKIN $SKIN */" >> $CSSFILE
		cat /var/www/skins/$SKIN/main.css >> $CSSFILE	
		#TODO Compress/cleanup file
	fi
	
	FILESIZE=`wc -c $CSSFILE |cut -f1 -d' '`
	ETAG=`md5sum $CSSFILE | cut -f1 -d' '`
	DATE=`date -R --date='last Fri'`
	EXPIRE=`date -R --date='next Fri'`
	echo "Vary: Accept-Encoding"
	echo "Cache-Control: max-age=3600, must-revalidate"
	echo "Date: $DATE"
	echo "Expires: $EXPIRE"
	echo "Last-Modified: $DATE"
	#echo "ETag: $ETAG"
	echo "Content-Length: $FILESIZE"
	echo "Content-Type: text/css"
	echo
	cat $CSSFILE
fi
