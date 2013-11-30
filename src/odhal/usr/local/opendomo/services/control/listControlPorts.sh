#!/bin/sh
#name:List controllers
#desc:List controllers
#package:odcontrol
#type:local

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

CTRLPATH="/var/opendomo/control"
CFGPATH="/etc/opendomo/control"
TMPFILE="/tmp/listcontrolports.tmp"

cd $CTRLPATH
#PORTS=`find -type p`

BNAME=`echo $1 | cut -d'-' -f1`
PNAME=`echo $1 | cut -d'-' -f2`
VALUE=$2   
# If "set" command is sent, execute
if test -n "$1"; then
	DNAME="$CTRLPATH/`echo $1 | sed 's/-/\//'`"
	if test -w $DNAME; then
		echo $2 > $DNAME
		# Some time to propagate changes for the driver
		usleep 100000 
#	else
#		echo "#ERR: Invalid port name"
	fi
fi
portsfound=0


if ! test -f $TMPFILE; then
	echo "#LOADING Loading..."
	# Here we generate a temporary file with all data required to read the ports.
	# The file format will be as follows:
	# "device/port	desc	way	values	tag	zone" > $TMPFILE
	touch $TMPFILE
	cd $CTRLPATH
	for device in *; do
		if test "$device" != "*"; then 
			cd $device
			for port in ?????; do
				if test "$port" != "?????"
				then
					portsfound=1
					zone=""
					tag=""
					way=""
					type=""
					desc=""
					if test -f $CFGPATH/$device/$port.desc; then
						desc=`cat $CFGPATH/$device/$port.desc`
					fi				
					if test -f $CFGPATH/$device/$port.info; then
						. $CFGPATH/$device/$port.info
					fi
					if test -z "$desc"; then
						desc="$port"
					fi
					if test -z "$tag"; then
						tag="none"
					fi		
					if test -z "$zone"; then
						zone="none"
					fi
					# Discarding disableds
					if test "$status" != "disabled"; then
						echo  "$device/$port	$desc		$values	type=$type	zone=$zone	tag=$tag	way=$way" | sed 's/ /+/g' >> $TMPFILE
					fi
				fi
			done
			cd ..
		fi
	done	
	# We sort it by tag (column 5)
	sort -k5 $TMPFILE -o $TMPFILE
fi


if test -d /etc/opendomo/tags; then
	cd /etc/opendomo/tags
else
	echo "#WARN: Tag file corrupted. Click Manage tags to fix it"
fi

# this function receives:

displayport() {
	if ! test -z "$1"; then
		if test -f /var/opendomo/control/$1.value
		then	
			value=`cat /var/opendomo/control/$1.value`
		else
			value="off"
		fi
		if test "$7" = "way=out"; then
			# Puerto de salida: ¿Qué valores puede tomar?
			ftype="subcommand[$3]"
		else
			# Si es un puerto de entrada, leerlo
			if test -z "$type"; then
				ftype="readonly"
			else
				ftype="readonly $type"
			fi
		fi		
		echo "	$1	$2	$ftype	$value" | sed 's/+/ /g' 
	fi
}

echo "#> Control ports"
echo "form:`basename $0`"
for TAG in * none; do
	if test -f /etc/opendomo/tags/$TAG ; then
		tname=`cat /etc/opendomo/tags/$TAG`
	else
		tname="$TAG"
	fi
	echo "	$TAG	$tname	separator"
	if touch $TMPFILE.$TAG; then
		if test -z $SELECTEDZONE; then
			grep tag=$TAG $TMPFILE > $TMPFILE.$TAG
		else
			grep tag=$TAG $TMPFILE | grep zone=$SELECTEDZONE > $TMPFILE.$TAG	
		fi
	fi
	#grep $TAG $TMPFILE | sed 's/^/# /'
	exec 3<$TMPFILE.$TAG
	while read LINE <&3
	do
		displayport $LINE
	done

done
chmod ugo+rw /tmp/listcontrolports* 2>/dev/null

echo "actions:"
if test -x /usr/local/opendomo/manageTags.sh; then
	echo "	manageTags.sh	Manage tags"
fi
if test -x /usr/local/opendomo/configureControlPorts.sh; then
	echo "	configureControlPorts.sh	Configure control ports"
fi
#if test `basename $0` = "listControlPorts.sh" && test -x /usr/local/opendomo/turnAllLightsOff.sh; then
#	echo "	turnAllLightsOff.sh	Turn all lights off"
#fi
echo 
