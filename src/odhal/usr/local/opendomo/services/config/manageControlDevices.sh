#!/bin/sh
#desc:Manage control devices
#package:odcontrol
#type:local

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later



CFGPATH="/etc/opendomo/control"
CTRLPATH="/var/opendomo/control"
PS=""
PORTS=""

mkdir -p $CFGPATH


case "$action" in
	"")

		# Available control devices
		echo "#> Available"
		echo "list:addControlDevice.sh	selectable"

		cd $CFGPATH
		EXISTS=0
		for i in *; do
			if test "$i" != "*"; then
				if ! echo $i | grep "\.conf" > /dev/null; then 
					echo "	-$i	$i	control device"
					EXISTS=1
				fi
			fi
		done
				
		if test "$EXISTS" = "0" ; then
			echo "# There are no control devices. Please, go to Add."
		fi

		echo "actions:"
		echo "	addControlDevice.sh	Add"
		echo "	delControlDevice.sh	Delete"
		echo "	configureControlPorts.sh	Configure control devices"
		echo

	;;
	"update")
		MODEL=$2
		PORT=`echo $3| tr - :`
		NAME=$4
		REFRESH=$5

		if test -z "$NAME"
		then
			echo "#ERR: Name cannot be empty."
			exit 1
		fi

		# TODO: verificar nombres existentes, solo al añadir
		if test -e $CFGPATH/$NAME.conf
		then
			echo "# Please choose another name."
			exit 1
		fi

		#if grep -q "$PORT" $CFGPATH/*.conf
		#then
		#	echo "#ERR: Device already in use"
		#	exit 1
		#fi

	 	echo "
device='$PORT'
driver='$MODEL'
dirname='$NAME'
refresh='$REFRESH'
" > $CFGPATH/$NAME.conf

		mkdir $CFGPATH/$NAME

		#echo "#INFO Device stored"
		# Restarting service
		/etc/init.d/odcontrol restart >/dev/null
		
		echo "#> Manage control devices"
		echo "list:`basename $0`"
		echo "# Device [$MODEL] configured"

		echo "actions:"
		echo "	configureControlPorts.sh	Configure control ports"
		echo "	manageControlDevices.sh	Manage control devices"

	;;
	
esac
echo
