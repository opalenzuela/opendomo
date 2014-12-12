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
	if ls * &>/dev/null; then
		for plugin in *.info; do
			AUTHORID=""
			source ./$plugin
			if ! test -z "$AUTHORID" && ! test -z "$AUTHOR"; then
				echo "	$AUTHORID	$DESCRIPTION	user	$AUTHOR"
			fi
	    	done
		echo "actions:"
		echo "	goback	Back"
        	echo
	else
        	echo "# No plugins installed yet"
		echo "actions:"
		echo "	goback	Back"
        	echo "	managePlugins.sh	Manage plugins"
        	echo
	fi
else
	echo "form:credits.sh"
	echo "	name	Name	readonly	$1"
	echo "	page	Page	application	https://github.com/$1"
	echo "actions:"
	echo "	goback	Back"
fi
echo
