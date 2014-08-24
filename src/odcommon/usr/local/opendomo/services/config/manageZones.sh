#!/bin/sh
#desc:Manage zones
#type:local
#package:odcommon

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

ZONEDIR=/etc/opendomo/zones
ETCDIR=/var/opendomo/cgiroot/map
ODDIR=/usr/local/opendomo

test -d $ZONEDIR || mkdir -p $ZONEDIR
test -d $ETCDIR || mkdir -p $ETCDIR

case $1 in
	"add"|"update")
		if test -z "$3"; then
			echo "#ERR Description field missing"
			exit 2
		fi
		if test -z "$2"; then
			code=`echo $3 | tr A-Z a-z | sed 's/[^a-z0-9]//g'`
		else 
			code="$2"
		fi
		touch $ZONEDIR/$code	
		echo "description='$3'" | sed 's/+/ /g' > $ZONEDIR/$code
		#echo "coords='$4'" >> $ZONEDIR/$code
		# Creamos el enlace dentro de ./map, con el script único
		#ln -s $ODDIR/services/syscript/displayZone.sh $ETCDIR/$code 2>/dev/null
		#coords=""
	;;
#	"delete")
#		rm -f $ZONEDIR/$2
#		rm -f $ETCDIR/$2
#	;;
	*)	# Probablemente estamos intentando editar una zona
		if test -n "$1" && test -f $ZONEDIR/$1; then
			. $ZONEDIR/$1
			echo "#> Editing [$description]"
			echo "form:`basename $0`"
			echo "	action	action	hidden	update"
			echo "	name	code	hidden	$1"
			echo "	desc	Description	text	$description"
			#echo "	coord	Coordinates	text	$coords"
			echo
			echo "#> Preview"
			echo "list:displayZone.sh	map"
			echo "	demozone	$1	zone"
			echo
			exit 0
		fi
	;;
esac

cd $ZONEDIR
echo "#> Current zones"
echo "list:`basename $0`	selectable"
for z in *; do
	if test "$z" != "*"; then
		if test -z $TAGS; then
			TAGS="$z"
		else
			TAGS="$TAGS,$z"
		fi
		. ./$z 2>/dev/null
		if test -z "$description"; then
			# file in old format
			description=`cat $z`
			echo "description='$description'" > $z 
		fi
		echo "	-$z	$description	zone"
		description=""
	fi
done
echo "actions:"
echo "	addZone.sh	Add zone"
echo "	delZone.sh	Delete zone"
echo
