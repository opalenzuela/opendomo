#!/bin/sh
echo "Content-type: text/xml"
echo
echo "<tile>"
echo "<visual lang='en-US' version='2'>"

date >> /var/opendomo/tmp/notification.log
CODE=2
TEXT="Finalmente!"

echo "<binding template='TileSquare150x150PeekImageAndText04' branding='name'>"
echo "<image id='$CODE' src='images/logo48.png'/>"
echo "<text id='$CODE'>$TEXT</text>"
echo "</binding>"

echo "<binding template='TileWide310x150ImageAndText01' branding='name'>"
echo "<image id='$CODE' src='images/logo48.png'/>"
echo "<text id='$CODE'>$TEXT</text>"
echo "</binding>"

echo "<binding template='TileSquare310x310ImageAndText01' branding='name'>"
echo "<image id='$CODE' src='images/logo300.png'/>"
echo "<text id='$CODE'>$TEXT</text>"
echo "</binding>"

echo "</visual>"
echo "</tile>"