#!/bin/sh
#desc:List control ports by zone
#package:odhal
#type:local

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

CTRLPATH="/var/opendomo/control"
CFGPATH="/etc/opendomo/control"

if ! test -d /var/opendomo/control_byzone ; then
	/usr/local/opendomo/organizeControlPorts.sh
fi



if ! test -z "$1"; then
	BNAME=`echo $1 | cut -d'-' -f1`
	PNAME=`echo $1 | cut -d'-' -f2`
	VALUE=$2   

	DNAME="$CTRLPATH/`echo $1 | sed 's/-/\//'`"
	if test -w $DNAME; then
		echo $2 > $DNAME
		# Some time to propagate changes for the driver
		usleep 100000 
	fi
fi
portsfound=0

cd /var/opendomo/control_byzone/

echo "#> Control ports"
echo "form:`basename $0`"
for zone in *; do

	#if test "$zone" != "*"; then
	
	if test -f /etc/opendomo/zones/$zone ; then
		tname=`cat /etc/opendomo/zones/$zone`
	else
		tname="$zone"
	fi
	echo "	$zone	$tname	separator"
	cd $CTRLPATH
	for port in *; do
		if test -f "$port" || test -p "$port"; then
			zone=""
			type=""
			name=""
			way=""
			tag=""
			status=""
			units=""
			values=""

			id="$device-$port"
			ftype="readonly"

					# Cargamos el tipo de puerto
					if test -f $CFGPATH/$device/$port.info; then
						. $CFGPATH/$device/$port.info

						if [ "$status" != "enabled" ]; then
							continue;
						fi

						# Si no está "name" definido, buscamos en ".desc"
						if test -z "$name"; then
							name=`cat $CFGPATH/$device/$port.desc 2>/dev/null`
						fi
						# Si no existe, el ID es el nombre del puerto
						if test -z "$name"; then
							name="$port"
						fi

						if test "$way" = "out"; then
							# Puerto de salida: ¿Qué valores puede tomar?
							ftype="subcommand[$values]"
							fvalue="$port.value"
							if test -f "$fvalue"; then
								value=`cat $fvalue`
							else
								value="off"
							fi

							# Mira si es justo lo que acabamos de pulsar
							if [ "$device" = "$BNAME" ]; then
								if [ "$port" = "$PNAME" ]; then
									value=$VALUE
								fi
							fi 

						else
							# Si es un puerto de entrada, leerlo
							value=`cat $port`
							if ! test -z "$units"; then
								value="$value $units"
							fi
						fi
					#else
					#	echo "#ERR: Port information file not found"
					#	echo "#ERR: $CFGPATH/$device/$port"
					#	exit 1
						if ! test -z "$type"; then
							ftype="$ftype $type"
						fi
						# Mostrar solo si no se filtra por zona o si la zona coincide
						if test -n "$SELECTEDZONE" && test "$SELECTEDZONE" = "$zone" || test -z "$SELECTEDZONE"; 
						then 
							if test "$tag" = "notassigned" && test -z "$tags"; then
								echo "	$id	$name	$ftype	$value"
							fi
						fi
					fi
				fi
			done
			cd ..
		fi
	done
done
#if test "$portsfound" = "0"; then
#	echo "#WARN: No ports found"
#fi
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
