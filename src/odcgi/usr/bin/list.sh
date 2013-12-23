#!/bin/sh
#desc:Lista el contenido del directorio pasado como argumento
#package:odcgi
#author:opalenzuela 
# 
# Este script muestra el contenido del directorio indicado como argumento, 
# formateado para el CGI de OpenDomo. Los elementos mostrados son, únicamente,
# aquellos que pueden ser ejecutados por el usuario validado (con el flag +x), 
# ya sean scripts o directorios.
# Por razones de seguridad, serán eliminados de la ruta combinaciones de dos
# puntos ".." y direccionadores estándar (<>)


D="$1"
# "control" dir by default
if test -z "$1" || test "/" = "$1"; then
	D="control/"
fi

RAIZ="."
RUTA="/var/opendomo/cgiroot"

cd $RUTA/$D
PLACE=`basename $D`
if test -f ./$PLACE.name; then
	PLACE=`cat ./$PLACE.name`
fi
if test -f ./../$PLACE.name; then
	PLACE=`cat ./../$PLACE.name`
fi
echo "#> Contents of [$PLACE]"
if test `basename $D` = "map"; then
	script="map.sh"
else
	script="list.sh"
fi
echo "list:$script	simple"

if ! test -f /etc/opendomo/udata/admin/email; then
	echo "#INFO OpenDomo is not yet configured. Click Configuration wizard to configure it"
	CFGWIZARD="1"
fi

for i in `find -maxdepth 1 -type d | cut -c 3-100 | sort && \
			 find -maxdepth 1 -type f | cut -c 3-100 | sort && \
			 find -maxdepth 1 -type l | cut -c 3-100 | sort \
			`; do
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
			if test -x "$i/run.sh"; then
				link="./$i/run.sh"
			else
				link="./$i/"
			fi
		else
			class="$base sh_file"                               
		fi

		if ! echo "$name" | grep "@invisible" > /dev/null
		then
			echo "	$link	$name  	$class"
		fi
    fi
done

echo "actions:"

if test "$CFGWIZARD" = "1"; then
	if test -x /usr/local/opendomo/wizFirstConfigurationStep1.sh; then
		echo "	wizFirstConfigurationStep1.sh	Configuration wizard"
	fi
fi

if test -x /usr/local/opendomo/addToFavorites.sh; then
	echo "	addToFavorites.sh	Add to Favorites"
fi
# Deprecated. Links are created automatically by oddiscovery
#if test -x /usr/local/opendomo/addInterfaceLink.sh; then
#	echo "	addInterfaceLink.sh	Add link"
#fi
#if test -x /usr/local/opendomo/delInterfaceLink.sh; then
#	echo "	delInterfaceLink.sh	Remove link"
#fi

echo
exit 0
