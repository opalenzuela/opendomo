#!/bin/sh
#desc:Select scene
#package:odcommon
#type:local

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

CFGPATH="/etc/opendomo/scenes"
CTRLPATH="/var/opendomo/control"

if ! test -d "$CFGPATH"; then
	mkdir -p "$CFGPATH" 2>/dev/null
fi

# If ONLY ONE argument is passed
if ! test -z "$1" && test -z "$2";then
	if test -f /tmp/lastscene.tmp; then
		lastscene=`cat /tmp/lastscene.tmp`
	else
		lastscene="nolastscenedefineD"
	fi
	
	if test "$1" = "$lastscene"; then
		echo "# Applying the same scene again. Ignored"
		echo
	fi
	
	if test -f $CFGPATH/$1.conf; then
		. $CFGPATH/$1.conf
		echo ""> /tmp/exitscene.tmp
		echo $1 > /tmp/lastscene.tmp
		if ! test -z "$values"; then
			desc=`grep '#desc' $CFGPATH/$1.conf | cut -f2 -d:`
			echo "#INFO Using [$desc]"
			for p in $values; do
				fil=`echo $p | cut -f1 -d= | sed -e 's/-/\//g' -e 's/_/\//g'`
				val=`echo $p | cut -f2 -d=`
				oldval=`cat $CTRLPATH/$fil.value`

				# We'll create a recovery script, that will save the states
				echo "echo $oldval > $CTRLPATH/$fil" >> /tmp/exitscene.tmp
				echo $val > $CTRLPATH/$fil
			done
		else
			echo "#ERR Invalid scene"
			exit 3
		fi
	else
		echo "#ERR Invalid scene"
		exit 2
	fi
fi

# No params, listing scenes
echo "#> Available scenes"
echo "list:`basename $0`	simple"
if test -d $CFGPATH; then
	cd $CFGPATH
	for i in *; do
		if test "$i" != "*"; then 
			code=`echo $i | cut -f1 -d.`
			desc=`grep desc: $i | cut -f2 -d:`
			echo "	-$code	$desc	scene"
		fi
	done
fi

if test -z "$code"; then
	if test -x /usr/local/opendomo/addScene.sh; then
		echo "# There are no scenes. Please, go to Add."
		echo "actions:"
		echo "	addScene.sh	Add"
	else
		echo "# No scenes were created. Please ask your installer."
	fi
	echo
	exit 0
fi
echo "actions:"
if test -x /usr/local/opendomo/manageScenes.sh; then
	echo "	manageScenes.sh	Manage scenes"
fi
if test -x /usr/local/opendomo/exitCurrentScene.sh; then
	echo "	exitCurrentScene.sh	Exit current scene"
fi
#echo "	delScene.sh	Delete scene"
echo
