#!/bin/sh
#desc:System information
#package:odbase
#group:users
#type:local

# Copyright(c) 2015 OpenDomo Services SL. Licensed under GPL v3 or later

## This menu shows the system information, with the installed plugins,  
## their versions and build time.

KERNELVERSION=`uname -r` 2> /dev/null

echo "#> About"
echo "list:`basename $0`"
echo "#INFO OpenDomoOS is free software, created by OpenDomo Services and the community and distributed under the GPLv3 license"
echo "#URL:http://www.gnu.org/licenses/gpl-3.0.html"
echo
echo "#> Installed products"
echo "list:`basename $0`	detailed"
echo "	-kernel 	Linux kernel	package	v$KERNELVERSION GPL v3"

ODBASE_VER=`apt-cache show odbase | grep Version | cut -f2 -d' '`
CLEANVER=`echo $ODBASE_VER | sed 's/\.//g'`
let CLEANVER="$CLEANVER + 1"
if wget -q --spider "http://es.opendomo.org/od$CLEANVER"
then
	echo "	-odbase 	OpenDomoOS base system 	package new	v$ODBASE_VER GPL v3"
	if test -x /usr/local/opendomo/updateSystem.sh; then
		echo "actions:"
		echo "	updateSystem.sh	Update system"
	fi
else
	echo "	-odbase 	OpenDomoOS base system 	package	v$ODBASE_VER GPL v3"
fi 
echo

# Checking plugins dir and see info
echo "#> Installed plugins"
echo "list:managePlugins.sh	detailed"
cd /var/opendomo/plugins/
for plugin in *.info
do
	if test -f $plugin
	then
		FOUND=1
		VERSION=""
		LICENSE="GPLv3"
		DESCRIPTION="$plugin plugin"
		source ./$plugin 
		PLUGINID=`echo $plugin | cut -f1 -d.`
		BUILD=`cat /var/opendomo/plugins/$PLUGINID.version`
		echo "	-$PLUGINID	$DESCRIPTION	package	$VERSION ($BUILD)"
	fi
done
if test -z "$FOUND"; then
	echo "# No plugins installed yet"
fi

echo "actions:"
echo "	credits.sh	Credits"
if test -x /usr/local/opendomo/managePlugins.sh; then
	echo "	managePlugins.sh	Manage plugins"
fi
echo
