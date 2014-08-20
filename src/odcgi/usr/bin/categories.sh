#!/bin/sh
#desc:Lista de categorías principales
#author:opalenzuela
#package:odcgi
#
# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

RUTA="/var/opendomo/cgiroot/"
test -f /etc/opendomo/uid || exit 0
echo "tabs:"
cd $RUTA
for i in control tools config; do
	if test -x $i && test -d $i ; then
		name="$i"
		if test -e "$i.name"; then
			name=`cat $i.name`
		fi
		case $1 in
			/${i}*)
				class=selected
			;;
			*)
				class=tab
			;;
		esac
		echo "	$i	$name	$class";		
	fi
done
echo 
echo 

