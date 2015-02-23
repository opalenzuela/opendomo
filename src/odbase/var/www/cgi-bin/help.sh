#!/bin/sh
echo "Content-type: text/html"
echo
TMPPATH="/var/opendomo/tmp/docs"
test -d "$TMPPATH"  || mkdir -p $TMPPATH

lang=`cat /etc/opendomo/lang`
TOPIC=`echo $QUERY_STRING | cut -f1 -d.`
echo "<html><head><link rel='stylesheet' type='text/css' media='screen' href='/cgi-bin/css.cgi?admin'>"
echo "<meta http-equiv='content-type' content='text/html; charset=utf-8'></head><body>"
echo "<fieldset id='helpbox'>"

TMPFILE="$TMPPATH/$TOPIC.tmp"
#if ! test -f $TMPFILE; then
	if test -f /usr/local/opendomo/docs/$TOPIC.$lang.txt; then
		cp /usr/local/opendomo/docs/$TOPIC.$lang.txt  $TMPFILE
	else 
		if test -f /usr/local/opendomo/docs/$TOPIC.txt; then
			cp /usr/local/opendomo/docs/$TOPIC.txt $TMPFILE
		else
			SCRIPT=/usr/local/opendomo/$TOPIC.sh
			if test -f "$SCRIPT" ; then
				grep '##' "$SCRIPT" | sed 's/\#//g' > $TMPFILE
				TOPIC=`head -n3 $SCRIPT | grep desc | cut -f2 -d:`
			else
				i18n.sh "Manual page not found" > $TMPFILE
			fi
		fi
	fi
#fi
echo "<legend>"
i18n.sh $TOPIC
echo "</legend>"
cat $TMPFILE | sed -e 's/href=\([^ ]*\) \([^$]*\)$/<a href=\1>\2<\/a>/' -e 's/^/<p>/' -e 's/$/<\/p>/' 

echo "</fieldset>"
echo "<div class='toolbar'>"
echo "<a class='button' href='http://www.opendomo.com/wiki/index.php?title=$TOPIC' target='_new'>"
i18n.sh "More information"
echo "</a>"
echo "</div>"
echo "</body></html>"
