#!/bin/sh
echo "Content-type: text/html"
echo
lang=`cat /etc/opendomo/lang`
TOPIC=`echo $QUERY_STRING | cut -f1 -d.`
echo "<html><head><link rel='stylesheet' type='text/css' media='screen' href='/cgi-bin/css.cgi?admin'></head><body>"
echo "<fieldset id='helpbox'>"
if test -f /usr/local/opendomo/docs/$TOPIC.$lang.txt; then
	cat /usr/local/opendomo/docs/$TOPIC.$lang.txt  | sed -e 's/^/<p>/' -e 's/$/<\/p>/'
else 
	if test -f /usr/local/opendomo/docs/$TOPIC.txt; then
		cat /usr/local/opendomo/docs/$TOPIC.txt | sed -e 's/^/<p>/' -e 's/$/<\/p>/'
	else
		echo "<legend>$TOPIC</legend>"
		i18n.sh "Manual page not found"
	fi
fi
echo "</fieldset>"
echo "<div class='toolbar'>"
echo "<a class='button' href='http://www.opendomo.com/wiki/index.php?title=$TOPIC' target='_new'>"
i18n.sh "More information"
echo "</a>"
echo "</div>"
echo "</body></html>"