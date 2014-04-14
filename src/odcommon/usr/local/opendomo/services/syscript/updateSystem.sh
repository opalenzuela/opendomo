#!/bin/sh
#desc: Update debian and opendomo packages

# Checking arch
if [ `arch` != "armv6l" ]; then
	ARCH="i386"
else
	ARCH="armhf"
fi

ODCOMPKG="odcommon_current_$ARCH.deb"
ODCGIPKG="odcgi_current_$ARCH.deb"
ODHALPKG="odhal_current_$ARCH.deb"
PKGURL="http://es.opendomo.org/files/"

LOGFILE="/var/opendomo/log/updateSystem.log"

APTFILE="/mnt/odconf/apt/lastupdate"
CURWEEK=`date +%W`

if [ `whoami` != "root" ]; then
	echo "#ERROR Only root can update system"
	exit 1
fi

#Only update apt sources once a week
if ! test -f $APTFILE || [ $CURWEEK != `cat $APTFILE` ]
then
    echo "#INFO Updating sources ..."
    sudo apt-get update
    date +%W >$APTFILE
fi

echo -n "#INFO Updating opendomo packages, please wait ..."
LC_ALL=C LANGUAGE=C LANG=C DEBIAN_FRONTEND=noninteractive sudo apt-get --force-yes -yq upgrade &>$LOGFILE
if wget $PKGURL/$ODCOMPKG $PKGURL/$ODCGIPKG $PKGURL/$ODHALPKG &>$LOGFILE; then
	if test sudo dpkg -i $ODCOMPKG $ODCGIPKG $ODHALPKG &>$LOGFILE; then
		rm $ODCOMPKG $ODCGIPKG $ODHALPKG
	else
		# Fix dpkg database installing missing deps
		LC_ALL=C LANGUAGE=C LANG=C DEBIAN_FRONTEND=noninteractive sudo apt-get --force-yes -fyq install &>$LOGFILE
		rm $ODCOMPKG $ODCGIPKG $ODHALPKG
	fi
fi
	echo "   (done)"
else
	echo "#ERROR Sources can't be downloaded, check network connection"
fi
