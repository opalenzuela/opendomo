#!/bin/sh
echo "Content-type: text/html"
echo
TOPIC=`echo $QUERY_STRING | cut -f1 -d.`
echo "<html><head><link rel='stylesheet' type='text/css' media='screen' href='/cgi-bin/css.cgi?admin'></head><body>"
if test -f /usr/local/opendomo/docs/$TOPIC.txt; then
	echo "<fieldset>"
	cat /usr/local/opendomo/docs/$TOPIC | sed 's/^/# /'
	echo "</fieldset>"
else
	echo "<fieldset>"
	echo "<legend>$TOPIC</legend>"
	i18n.sh "Manual page not found"
	echo "</fieldset>"
fi
echo "<div class='toolbar'><a href='http://www.opendomo.com/wiki/index.php?title=$TOPIC' target='_new'>"
i18n.sh "More information"
echo "</a></div>"
echo "</body></html>"