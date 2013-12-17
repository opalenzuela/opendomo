#!/bin/sh

# Variables

SCRIPTSDIR="scripts"
. $SCRIPTSDIR/sdk_variables.sh

# Force image dir
IMAGEDIR="$TMPDIR/image.arm"

# Check variable
if test -z $1; then
	echo "ERROR: You need select SD to export"
	exit 1
else
	if test -e $1; then
		DEVICE="$1"
	else
		DEVICE="/dev/$1"
		if ! test -e $DEVICE; then
			echo "ERROR: Device don't exist"
			exit 1
		fi
	fi
fi

# Copy RaspberryPi boot files
cp -r $RPIFILESDIR/* $IMAGEDIR/

# Exporting to SD
echo "INFO: Export opendomo to SD ($DEVICE)..."
echo -n "WARN: This action delete all data in $DEVICE, are you sure? (y/n): "
read ASK

if [ "$ASK" = "y" ]; then
	# Umount drive
	umount $DEVICE 2>/dev/null

	if
	mkfs.vfat -I -n "opendomo" $DEVICE 2>/dev/null >/dev/null
	then
		# Mount and Copy files
		mount $DEVICE $MOUNTDIR
		cp -r $IMAGEDIR/* $MOUNTDIR/
	else
		echo "ERROR: Device $DEVICE can be mounted"
	fi
else
	exit 1
fi
