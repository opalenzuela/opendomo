#!/bin/sh
#desc:System information
#package:odbase
#group:users
#type:local

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

KERNELVERSION=`uname -r` 2> /dev/null

echo "#> About"
echo "list:`basename $0`"
echo "#INFO OpenDomoOS is free software, created by OpenDomo Services and the community and distributed under the GPLv3 license"
echo "#URL:http://www.gnu.org/licenses/gpl-3.0.html"
echo
echo "#> Installed products"
echo "list:`basename $0`	detailed"

ODBASE_VER=`apt-cache show odbase | grep Version| cut -f2 -d' '`

echo "	-kernel 	Linux kernel	package	v$KERNELVERSION GPL v3"
echo "	-odbase 	OpenDomoOS base system 	package	v$ODBASE_VER GPL v3"
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
		echo "	-$plugin	$DESCRIPTION	package	$VERSION $LICENSE"
	fi
done
if test -z "$FOUND"; then
	echo "# No plugins installed yet"
	echo "actions:"
	echo "	credits.sh	Credits"
	echo "	managePlugins.sh	Manage plugins"
fi
echo
