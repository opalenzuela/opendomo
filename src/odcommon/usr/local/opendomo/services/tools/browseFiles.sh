#!/bin/sh
#dname:Browse files and folders
#desc:Browse files and folders
#package:odcommon
#type:local

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

ODPATH="/usr/local/opendomo"

if test -z "$EXT"; then
	EXT=""
fi

if test -z "$PATTERN"; then
	if test -n "$EXT"; then
		PATTERN="*.$EXT"
	else
		PATTERN="*"
	fi
else
	if test -z "$EXT"; then
		EXT=`echo $PATTERN | cut -f2 -d.`
	fi
fi

# El primer argumento contiene la ruta que va a ser explorada. Esta ruta se 
# encuentra necesariamente, por razones de seguridad, dentro de /media.

if test "$1" = ""  && test "$STARTPATH" != "" ; then
   RUTA="$STARTPATH";
else
   RUTA="$1"
fi


if test -d "/media/$RUTA"; then
	cd "/media/$RUTA"
	if test "$RUTA" = "" ; then
		RUTA="/"
		echo "#> Select drive"
	else
		echo "#> Contents of [$RUTA]"
	fi

	echo "list:`basename $0`	iconlist selectable"
	#for i in `find . -maxdepth 1 ! -iname '.*' $OPTIONS | sed 's/.\///g'`; do
	for i in *
	do
		if test "$i" != "*"; then
			e=`echo "$i" | cut -f2 -d.`
			dn=`echo "$i" | sed 's/^.\///'`
			bn=`basename "$i"`

			if test -d "$i"; then
				if test "$i" != "sysconf"; then
					# Directorios (todos)
					if test "$RUTA" != "/"; then
						echo "	-$RUTA/$dn	$i	dir	"
					else
						echo "	-/$dn	$i	dir	"
					fi
				fi
			else
				# Archivos (si coinciden con EXT o si EXT="")
				if test "$e" = "$EXT" || test -z "$EXT" ; then
					case "$e" in 
						jpg|jpeg|png|gif)
							type="file image"
						;;
						avi|mpeg|mpg|mov|mp4|mjpg|webm|ogg)
							type="video"
						;;
						txt|me|ME)
							type="file text"
						;;
						htm|html)
							type="file html"
						;;
						
						*)
							type="file"
						;;
					esac
					if ! test -z "$PREFIX"
					then
						dn=`echo "$dn" | grep "^$PREFIX"`
					fi
					if ! test -z "$SUFFIX"
					then
						dn=`echo "$dn" | grep "$SUFFIX"`
					fi
					if ! test -z "$dn"
					then
						echo "	-$RUTA/$dn	$i	$type	"
					fi

				fi
			fi
		else
			echo "#WARN This location is empty"
		fi
	done
	echo "actions:"
	if test -x $ODPATH/sendFileByEmail.sh; then
		echo "	sendFileByEmail.sh	Send file by email"
	fi
	if test -x $ODPATH/archiveFile.sh; then
		echo "	archiveFile.sh	Archive file"
	fi
	if test -x $ODPATH/bgmusicAddDBEntry.sh; then
		echo "	bgmusicAddDBEntry.sh	Add to playlist"
	fi
	if test -x $ODPATH/addResourceToBackup.sh; then
		echo "	addResourceToBackup.sh	Add to backup"
	fi
	if test -x $ODPATH/deleteFiles.sh; then
		echo "	deleteFiles.sh	Delete"
	fi
	
	echo 
else 
	echo "#ERR Invalid path [$RUTA]"
fi
echo
