#!/bin/sh
#desc:System information
#package:odbase
#type:local

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

KERNELVERSION=`uname -r` 2> /dev/null

echo "#> About"
echo "list:`basename $0`"
echo "# OpenDomoOS is free software, created by OpenDomo Services and the community and distributed under the GPLv3 license"
echo "#URL:http://www.gnu.org/licenses/gpl-3.0.html"
echo
echo "#> Installed products"
echo "list:`basename $0`	detailed"

ODBASE_VER=`apt-cache show odbase | grep Version| cut -f2 -d' '`

echo "	-kernel 	Linux kernel	package	v$KERNELVERSION GPL v3"
echo "	-odbase 	OpenDomoOS base system 	package	v$ODBASE_VER GPL v3"
echo

echo "#> Installed plugins"
echo "list:managePlugins.sh	detailed"
cd /var/opendomo/plugins/

# Checking plugins dir and see info

for plugin in `ls | cut -f1 -d. | uniq`; do
	if iscfg.sh $plugin.info &>/dev/null; 
	then
		VERSION=""
		LICENSE="GPLv3"
		DESCRIPTION="$plugin plugin"
		source ./$plugin.info  2> /dev/null
		echo "	-$plugin	$DESCRIPTION	package	$VERSION $LICENSE"
	else
		echo "	-$plugin	$plugin 	package 	Missing info file "
	fi
done
echo "actions:"
echo
