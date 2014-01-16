#!/bin/sh
#desc: Update debian and opendomo packages

# Checking arch
if [ `arch` != "armv6l" ]; then
	ARCH="i386"
else
	ARCH="armhf"
fi

# Packages and url
ODCOMPKG="odcommon_current_$ARCH.deb"
ODCGIPKG="odcgi_current_$ARCH.deb"
ODHALPKG="odhal_current_$ARCH.deb"
PKGURL="http://es.opendomo.org/files/"

if [ `whoami` != "root" ]; then
	echo "#ERR Only root can update system"
	exit 1
fi

# Update system
cd /tmp
echo "#INF Updating opendomo packages ..."
if apt-get update; then
	apt-get --force-yes -yq upgrade
	if wget $PKGURL/$ODCOMPKG $PKGURL/$ODCGIPKG $PKGURL/$ODHALPKG; then
		dpkg -i $ODCOMPKG $ODCGIPKG $ODHALPKG
		rm $ODCOMPKG $ODCGIPKG $ODHALPKG
	fi
else
	echo "#ERR Sources can't be downloaded, check network connection"
fi
