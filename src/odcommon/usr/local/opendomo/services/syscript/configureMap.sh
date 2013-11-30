#!/bin/sh
#desc:Configure map
#type:local
#package:odcommon

if test -f /etc/opendomo/map.conf
then
	. /etc/opendomo/map.conf
else
	echo "#ERROR No map file found "
	exit 1
fi
SYSTEMID=`cat /etc/opendomo/uid`
URL="http://cloud.opendomo.com/mapcf"

if ! touch "/media/$FILE" ; then
	echo "#ERROR Missing privileges"
	exit 1
fi

if test -z "$1"; then
	echo "form:configureMap.sh	wizard"
	echo "	stepfrom	stepfrom	hidden	1"
	echo "	maptool	Map	application	$URL/index.php?UID=$SYSTEMID"
	echo "actions:"
	echo "	configureMap.sh	Next"
else
	if test "$1" = "update"; then
		CFGFILE="/etc/opendomo/map.conf"
		echo "FILE=$FILE" > $CFGFILE 
		echo 'URL="/cgi-bin/od.cgi/tools/browseFiles.sh?FILE=$FILE&GUI=none"' >> $CFGFILE
		echo "WIDTH=$imagw" >> $CFGFILE		
		echo "HEIGHT=$imagh" >> $CFGFILE
		echo "#INF: File updated"
		echo	
	else
		echo "form:configureMap.sh	wizard"
		if wget $URL/files/$SYSTEMID.png -O /media/$FILE -q
		then
			#TODO We should obtain the dimensions of the image
			echo "	funct	funct	hidden	update"
			echo "	image	Image	image	/cgi-bin/od.cgi/tools/browseFiles.sh?FILE=$FILE&GUI=none"
			echo "	imagw	width	text 	300"
			echo "	imagh	height	text 	200"
			echo
		else
			echo "#ERROR Server cannot be reached"
			exit 2
		fi
		echo "actions:"
		echo "	manageZones.sh	Manage zones"
	fi
fi
echo
