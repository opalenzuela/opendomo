#!/bin/sh
#desc This script will update all the required JS in a single HTTP query

# Copyright(c) 2015 OpenDomo Services SL. Licensed under GPL v3 or later

echo "Content-Type: text/javascript"
echo "Cache-Control: max-age=3600"
echo "Vary: Accept-Encoding"
echo

cd /var/www/scripts/common/
for file in *.js
do
	echo "/* File $file*/"
	cat $file
	echo
done
