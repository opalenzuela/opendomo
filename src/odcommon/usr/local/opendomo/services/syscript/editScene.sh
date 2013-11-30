#!/bin/sh
#desc:Edit scene
#package:odhal
#type:local

# Copyright(c) 2012 OpenDomo Services SL. Licensed under GPL v3 or later

CFGPATH="/etc/opendomo/scenes"
CTRLPATH="/var/opendomo/control_byzone"
PCFGPATH="/etc/opendomo/control"

if test -z "$1"; then
	echo "#WARN No scene was specified"
	echo
	exit 0
fi

if test -f $CFGPATH/$1.conf; then
	if test "$2" = "save"; then
		. $CFGPATH/$1.conf
		#echo "# values=$values"
		#echo "# plist=$plist"
		for p in $values; do
			pname=`echo $p | cut -f1 -d=`
			pnamemodif=`echo $pname | sed 's/-/_/'`
			eval newval='$'$pnamemodif
			newvalues="$newvalues $pname=$newval"
		done
		#echo "# $newvalues"
		echo "#desc:$desc "> $CFGPATH/$1.conf
		echo "desc='$desc'" >> $CFGPATH/$1.conf 
		echo "plist='$plist'" >> $CFGPATH/$1.conf
		echo "values='$newvalues'" >> $CFGPATH/$1.conf
		echo "#INFO Changes saved"
	fi
	
	. $CFGPATH/$1.conf
	echo "#> Editing scene [$desc]"
	echo "form:editScene.sh"
	echo "	:	By default, a scene is created with the state that the ports have in the creation moment 	:"
	echo "	:	You can modify the individual state of each device in this page	:"
	echo "	file	hidden	hidden	$1"
	echo "	action	action	hidden	save"
	for p in $values; do
		pname=`echo $p | cut -f1 -d= | sed 's/-/_/'`
		valselected=`echo $p | cut -f2 -d=`
		fname=`echo $pname | sed 's/_/\//'`
		if test -f $PCFGPATH/$fname.desc; then
			pdesc=`cat $PCFGPATH/$fname.desc`
			# $PCFGPATH/$fname.info
		else
			echo " file $PCFGPATH/$fname.desc not found"
		fi

		# Should NOT happen. Just in case
		test -z "$pdesc" && 	pdesc=$pname
	
		#TODO Use the values from the configuration file, instead of on/off
		echo	"	$pname	$pdesc	list[on,off]	$valselected"
	done
	echo "actions:"
	echo "	editScene.sh	Save changes"
	echo "	setScene.sh	Back to list"
	echo
else
	echo "#ERR Scene [$1] not found"
	exit 1
fi
