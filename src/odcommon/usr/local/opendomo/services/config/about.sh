#!/bin/sh
#desc:About
#package:odcgi
#type:local

# Copyright(c) 2012 OpenDomo Services SL. Licensed under GPL v3 or later

KERNELVERSION=`uname -r` 2> /dev/null
BUSYBOXVERSION=`busybox --help | head -n 1 | cut -f 2 -d' '` 2>/dev/null

if test -f /etc/VERSION; then
	ODVERSION=`cat /etc/VERSION`
else
	ODVERSION=`cat /mnt/system/VERSION`
fi

echo "#> About"          
echo "list:`basename $0`"     
echo "#INF: OpenDomo v.[$ODVERSION]"
echo "# OpenDomo is free software, created by OpenDomo Services and the community and distributed under the GPLv3 license"
echo "#URL:http://www.gnu.org/licenses/gpl-3.0.html"
echo


if test -z "$1"; then
	echo "#> Installed products"
	echo "list:`basename $0`	detailed"
	echo "	-kernel	Linux kernel	package	v.$KERNELVERSION GPL v3"
	echo "	-busybox	Busybox 	package	$BUSYBOXVERSION GPL v3"
	cd /var/pkg/installed/
	for pkg in *; do
		if test "$pkg" != "kernel" && test "$pkg" != "*"; then
			VER=`cat $pkg` 2>/dev/null
			if test -r /var/pkg/$pkg.lic; then
				LICENSE=`head -1 /var/pkg/$pkg.lic` 
			else
				LICENSE=""
			fi
			if test -r /var/pkg/$pkg.desc; then
				DESC=`head -1 /var/pkg/$pkg.desc`
			else
				DESC=""
			fi
			if ! test -z "$DESC" && ! test -z "$LICENSE"; then
				echo "	-$pkg	$DESC	package	v.$VER $LICENSE"
			fi
		fi
	done
	echo "actions:"
else
	echo "#> License information for [$1]"
	echo "form:about.sh"
	if test -r /var/pkg/$1.lic; then
		sed 's/^/#/' /var/pkg/$1.lic
	else
		echo "#WARN No license file was found"
	fi
	echo "actions:"
	echo "	about.sh	Back"
fi
echo


