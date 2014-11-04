#!/bin/sh
#desc This script will update all the required JS in a single HTTP query

echo "Content-Type: text/javascript"
echo "Cache-Control: max-age=3600"
echo
cd /var/www/scripts/common/
for file in *.js
do
	echo "/* File $file*/"
	cat $file
	echo
done