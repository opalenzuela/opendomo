#!/bin/sh
#desc:Lists the content of the directory
#package:odbase
#author:opalenzuela
#
# Este script muestra el contenido del directorio indicado como argumento,
# formateado para el CGI de OpenDomo. Los elementos mostrados son, unicamente,
# aquellos que pueden ser ejecutados por el usuario validado (con el flag +x),
# ya sean scripts o directorios.
# Por razones de seguridad, seran eliminados de la ruta combinaciones de dos
# puntos ".." y direccionadores estandar (<>)

# Copyright(c) 2015 OpenDomo Services SL. Licensed under GPL v3 or later
 
D="$1"
# "control" dir by default
if test -z "$1" || test "/" = "$1"; then
	D="control/"
fi

RAIZ="."
RUTA="/var/opendomo/cgiroot"

cd $RUTA
cd $RUTA/$D
PLACE=`basename $D`
if test -f $RUTA/$PLACE.name; then
	PLACE=`cat $RUTA/$PLACE.name`
fi

echo "#> [$PLACE]"
script="list.sh"

echo "list:list.sh	listbox"

UIDFILE="/etc/opendomo/uid"

if ! test -f $UIDFILE; then
	echo "#INFO OpenDomoOS is not yet configured."
	CFGWIZARD="1"
	if test -x /usr/local/opendomo/wizFirstConfigurationStep1.sh; then
		/usr/local/opendomo/wizFirstConfigurationStep1.sh
		exit 0
	fi
fi

if test $USER != "admin" && ! test -f /home/$USER/.email; then
	echo "#INFO Welcome to OpenDomoOS. Before starting, I need some more data to create your account."
	/usr/local/opendomo/userModify.sh
	exit 0
fi

link=""
for i in *.sh; do
	link="./$i"
	if test -x "$i"; then
		if test -e "$i.name"; then
			name=`cat $i.name`
		else
			name="$i"
		fi

		if test -z "$name"; then
			name="$i"
		fi

		if test -d $i; then
			class="$base dir"
			link="./$i/"
		else
			class="$base sh_file"
		fi

		echo "	$link	$name  	$class"
    fi
done

if test -z "$name"
then
	echo "#INFO This location is empty"
fi

echo "actions:"
if test "$CFGWIZARD" = "1"; then
	if test -x /usr/local/opendomo/wizFirstConfigurationStep1.sh; then
		echo "	wizFirstConfigurationStep1.sh	Configuration wizard"
	fi
fi

if test -x /usr/local/opendomo/addToFavorites.sh; then
	echo "	addToFavorites.sh	Add to Favorites"
fi

echo
exit 0
