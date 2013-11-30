#!/bin/sh
#desc:Launch sequence
#type:local
#package:odcommon

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

SEQPATH="/etc/opendomo/sequences"
if ! test -d $SEQPATH; then
	mkdir -p $SEQPATH 2>/dev/null
fi


if test -z "$1"; then
	echo "#> Available sequences"
	echo "list:`basename $0`	simple"
	EXIST=0
	if !  test -d $SEQPATH; then
		mkdir $SEQPATH
	fi
	cd $SEQPATH 
	for i in *; do
		if test "$i" != "*"; then
			if test -x $i; then
				desc=`grep '#desc' $i | cut -f2 -d:` 2>/dev/null
				if test -z "$desc"; then
					desc=$i
				fi
				echo "	-$i	$desc	sequence"
				EXIST=1
			fi
		fi
	done
	if test "$EXIST" = "0" ; then
		if test -x /usr/local/opendomo/addSequence.sh; then
			echo "# There are no sequences. Please, go to Add."
			echo "actions:"
			echo "	addSequence.sh	Add"
		else
			echo "# There are no sequences. Please ask your installer."
		fi
		echo
		exit 0
	fi

	echo "actions:"
	if test -x /usr/local/opendomo/manageSequence.sh; then
		echo "	manageSequence.sh	Manage sequences"
	fi
else
	# SEQUENCE REQUESTED
	if test -x "$SEQPATH/$1"; then
		if ! test -f /var/run/$1.pid ; then
			touch /var/run/$1.pid
			desc=`grep '#desc' "$SEQPATH/$1" | cut -f2 -d:` 2>/dev/null
			echo "#INFO Launching [$desc]"
			/bin/logevent notice odcommon "Sequence [$desc] started"
			bgshell "$SEQPATH/$1 && /bin/logevent notice odcommon 'Sequence finished' && rm /var/run/$1.pid"
	
			/usr/local/opendomo/launchSequence.sh
		else
			/bin/logevent debug odcommon "Sequence [$1] is already active. Ignoring."
		fi
	else
		echo "#ERROR Missing privileges"
		exit 2
	fi
fi
echo
