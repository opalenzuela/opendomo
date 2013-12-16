#!/bin/sh
#desc:Lista de categorías principales
#author:opalenzuela
#package:odcgi
#
# Este script genera el listado de elementos que formará las pestañas de la 
# interfaz web de OpenDomo

RUTA="/var/opendomo/cgiroot/"
echo "tabs:"
cd $RUTA
for i in control map tools config; do
	if test -x $i && test -d $i ; then
		name="$i"
		if test -e "$i.name"; then
			name=`cat $i.name`
		fi
		cd $i
		N=`ls *.sh | wc -l` 2>/dev/null
		case $1 in
			/${i}*)
				class=selected
			;;
			*)
				class=tab
			;;
		esac
		if test "$N" = "1"; then
			i="$i/`ls *.sh`"
		fi
		if test -d $RUTA/$i; then
			echo "	$i/	$name	$class";
		else
			echo "	$i	$name	$class";		
		fi

		cd ..
	fi
done
echo 
echo 

