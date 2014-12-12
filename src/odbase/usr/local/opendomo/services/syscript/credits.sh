#!/bin/sh
#desc:Credits
#package:odbase
#group:users
#type:local

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

if test -z "$1"; then

	echo "#> OpenDomoOS Credits"
	echo "list:credits.sh	detailed"
	echo "	dlerch  	Daniel Lerch    	user	Project founder"
	echo "	opalenzuela	Oriol Palenzuela	user	Lead developer"
	echo "	icatalan	Isidro Catalan  	user	Cloud designer"
	echo "	jmirasb 	Javier Miras    	user	Software architect"
	echo "	inigoalonso	Iñigo Alonso    	user	Logo and graphic design"
	echo
	echo "#> Plugin credits"
	echo "list:credits.sh	detailed"
	cd /var/opendomo/plugins/
	for plugin in *.info; do
		if test -f $plugin; then
			AUTHORID=""
			source ./$plugin
			if ! test -z "$AUTHORID" && ! test -z "$AUTHOR"; then
				echo "	$AUTHORID	$DESCRIPTION	user	$AUTHOR"
			fi
		fi
	done
	if test -z "$AUTHORID"; then
		echo "#INFO No plugins installed"
		echo "actions:"
		echo "	goback	Back"
		if test -x /usr/local/opendomo/managePlugins.sh; then
			echo "	managePlugins.sh	Manage plugins"
		fi
		echo
		exit 0
	fi	
	echo "actions:"
	echo "	goback	Back"
else
	echo "form:credits.sh"
	echo "	name	Name	readonly	$1"
	echo "	page	Page	application	https://github.com/$1"
	echo "actions:"
	echo "	goback	Back"
fi
echo
