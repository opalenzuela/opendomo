#!/bin/sh
echo "Content-type: text/xml"
echo
if test -f /var/www/data/notifications.xml 
then
	cat /var/www/data/notifications.xml 
fi
