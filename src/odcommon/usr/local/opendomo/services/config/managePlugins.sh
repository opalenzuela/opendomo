#!/bin/sh
#desc:Manage plugins
#type:local
#package:odcommon

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

TMPDIR="/var/opendomo/tmp"
OSVER=`cat /etc/VERSION`
OSVER="2.0.0"
REPOSITORY="http://cloud.opendomo.com/repo/$OSVER/"
if ! test -f $TMPDIR/repo.lst; then
	if wget $REPOSITORY -O $TMPDIR/repo.lst --no-check-certificate --max-redirect=0 2>/dev/null
	then
		echo "#INFO Repository updated"
	else
		echo "#ERROR Cannot find repository for version [$OSVER]"
		exit 1
	fi
fi
if test -z "$1"; then
	echo "#> Manage plugins"
	echo  "list:managePlugins.sh	iconlist"
	for p in `grep -v "#" $TMPDIR/repo.lst | cut -f1 -d- | uniq`; do
		ID=`grep $p $TMPDIR/repo.lst | cut -f1 -d'-' | head -n1`
		DESC=`grep $p $TMPDIR/repo.lst | cut -f3 -d';' | head -n1`
		if test -f /var/opendomo/plugins/$ID
		then
			echo "	-$ID	$DESC	plugin new"
		else
			echo "	-$ID	$DESC	plugin installed"
		fi
	done
	if test -z "$ID"; then
		echo "#ERROR The repository was empty. Try again later"
	fi
	echo "actions:"
	echo "	managePlugins.sh   Details"

else
	# Parameter was passed (requesting plugin's details)
	URL=`grep ^$1 $TMPDIR/repo.lst | sort | head -n1 | cut -f2 -d';' `
	DESC=`grep ^$1 $TMPDIR/repo.lst | sort | head -n1 | cut -f3 -d';' `
	DEPS=`grep ^$1 $TMPDIR/repo.lst | sort | head -n1 | cut -f4 -d';' `
	if test -z "$DEPS"; then
		DEPS="none"
	fi
	echo "#> Plugin details"
	echo "form:managePlugins.sh"
	echo "	code	Code	readonly	$1"
	echo "	desc	Description	readonly	$DESC"
	echo "	deps	Dependences	readonly	$DEPS"
	echo "	icon	Icon	image	$1.png"
	echo "actions:"
	if ! test -f /var/opendomo/plugins/$1; then
		echo "	installPlugin.sh	Install"
	else
		echo "	removePlugin.sh	Uninstall"
	fi
	echo "	goback	Back"
	echo "	updatePlugins.sh	Update all"
fi
echo