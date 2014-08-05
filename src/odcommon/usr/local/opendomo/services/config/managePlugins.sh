#!/bin/sh
#desc:Manage plugins
#type:local
#package:odcommon

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

TMPDIR="/var/opendomo/tmp"

if test -z "$1"; then
	# This file should be created by the update script
	if ! test -f $TMPDIR/repo.tmp 
	then
		source /etc/os-release
		REPOSITORY="http://cloud.opendomo.com/repo/$VERSION"
		if ! wget $REPOSITORY/ -O $TMPDIR/repo.tmp --no-check-certificate --max-redirect=0 2>/dev/null
		then
			echo "#ERR: The is not available now. Try again later"
		fi
	fi
	touch $TMPDIR/repo.tmp
	if ! test -f $TMPDIR/repo.lst
	then
		# Filtering wrong format
		grep "-" $TMPDIR/repo.tmp > $TMPDIR/repo.lst
	fi

	echo "#> Manage plugins"
	echo  "list:managePlugins.sh	iconlist"
	
	for p in `grep -v "#" $TMPDIR/repo.lst | cut -f1 -d- | uniq`; do
		ID=`grep $p $TMPDIR/repo.lst | cut -f1 -d'-' | head -n1`
		DESC=`grep $p $TMPDIR/repo.lst | cut -f3 -d';' | head -n1`
		test -z "$DESC" && DESC="$ID"
		if test -f "/var/opendomo/plugins/$ID.version"
		then
			echo "	-$ID	$DESC	image plugin installed"
		else
			if test -f /var/opendomo/run/install-$ID.error
			then
				echo "	-$ID	$DESC	image plugin failed"
			else
				echo "	-$ID	$DESC	image plugin new"
			fi
		fi
	done
	if test -z "$ID"; then
		echo "#ERR: The repository was empty. Try again later"
	fi
	echo "actions:"
	echo "	managePlugins.sh	Details"
	echo "	updateSystem.sh	System update"
else
	# Parameter was passed (requesting plugin's details)
	URL=`grep ^$1 $TMPDIR/repo.lst  | sort -r | head -n1 | cut -f2 -d';' `
	DESC=`grep ^$1 $TMPDIR/repo.lst | sort -r | head -n1 | cut -f3 -d';' `
	DEPS=`grep ^$1 $TMPDIR/repo.lst | sort -r | head -n1 | cut -f4 -d';' `
	WEB=`grep ^$1 $TMPDIR/repo.lst  | head -n1 | cut -f6 -d';' `
	if test -z "$DEPS"
	then
		DEPS="none"
	fi
	if test -z "$WEB"
	then
		# If website is not specified, we use the community one. We expect
		# to have a valid icon in PNG with the package's ID as filename.
		WEB="http://es.opendomo.org/files"
	fi
	echo "#> Plugin details"
	echo "form:managePlugins.sh"
	echo "	code	Code	readonly	$1"
	echo "	desc	Description	readonly	$DESC"
	echo "	deps	Dependences	readonly	$DEPS"
	echo "	webp	Web page	readonly	$WEB"
	echo "	icon	Icon	image	$WEB/$1.png"
	echo "actions:"
	if ! test -f "/var/opendomo/plugins/$1.version"; then
		echo "	installPlugin.sh	Install"
	else
		echo "	removePlugin.sh	Uninstall"
	fi
	echo "	goback	Back"
	echo "	updatePlugins.sh	Update all"
fi
echo
