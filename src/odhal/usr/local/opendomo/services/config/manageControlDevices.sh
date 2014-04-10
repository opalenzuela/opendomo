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
		port=`echo $port| tr - :`

		if test -z "$dirname"
		then
			echo "#ERR: Name cannot be empty."
			exit 1
		fi

		# TODO: verificar nombres existentes, solo al añadir
		if test -e $CFGPATH/$dirname.conf
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
device='$port'
driver='$model'
dirname='$dirname'
refresh='$refresh'
username='$username'
password='$password'
ip='$ip'
" > $CFGPATH/$NAME.conf

		mkdir $CFGPATH/$NAME

		#echo "#INFO Device stored"
		# Restarting service
		/usr/local/opendomo/daemons/odhal restart >/dev/null
		
		echo "#> Manage control devices"
		echo "list:`basename $0`"
		echo "# Device [$model] configured"

		echo "actions:"
		echo "	configureControlPorts.sh	Configure control ports"
		echo "	manageControlDevices.sh	Manage control devices"

	;;
	
esac
echo
