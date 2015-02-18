#!/bin/sh
#desc:Credits
#package:odbase
#group:users
#type:local

# Copyright(c) 2015 OpenDomo Services SL. Licensed under GPL v3 or later

if test -z "$1"; then

	echo "#> OpenDomoOS Credits"
	echo "list:credits.sh	detailed"
	echo "	dlerch  	Daniel Lerch    	user	Project founder"
	echo "	opalenzuela	Oriol Palenzuela	user	Lead developer"
	echo "	icatalan	Isidro Catalan  	user	Cloud designer"
	echo "	jmirasb 	Javier Miras    	user	Software architect"
	echo "	inigoalonso	Iñigo Alonso    	user	Logo and graphic design"
	echo
	echo "#> Installed plugins"
	echo "list:credits.sh	detailed"
	cd /var/opendomo/plugins/
	for plugin in *.info; do
		if test -f $plugin; then
			BNAME=`echo $plugin | cut -f1 -d.`
			FOUND=1
			AUTHORID=""
			source ./$plugin
			echo "	sep$plugin	$DESCRIPTION	separator"
			if ! test -z "$AUTHORID" && ! test -z "$AUTHOR"; then
				echo "	$AUTHORID	Project leader	user leader	$AUTHOR"
			fi
			if test -f /var/opendomo/plugins/$BNAME.credits
			then
				cat /var/opendomo/plugins/$BNAME.credits
			fi
		fi
	done
	if test -z "$FOUND"; then
		echo "#INFO No plugins installed yet"
	fi	
	echo "actions:"
	echo "	goback	Back"
	if test -x /usr/local/opendomo/managePlugins.sh; then
		echo "	managePlugins.sh	Manage plugins"
	fi	
else
	echo "form:credits.sh"
	echo "	name	Name	readonly	$1"
	echo "	page	Page	application	https://github.com/$1"
	echo "actions:"
	echo "	goback	Back"
fi
echo
