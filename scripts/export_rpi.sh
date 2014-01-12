#!/bin/sh

# Variables

SCRIPTSDIR="scripts"
. $SCRIPTSDIR/sdk_variables.sh

# Force image dir
TARGETDIR="$TMPDIR/image.arm"

# Creating image folders
mkdir -p $TARGETDIR/sysconf $TARGETDIR/plugins

# Check variable
# TODO variable device need a drive no partition
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
cp -r $RPIFILESDIR/* $TARGETDIR/

# Exporting to SD
echo "INFO: Export opendomo to SD ($DEVICE)..."
echo -n "WARN: This action delete all data in $DEVICE, are you sure? (y/n): "
read ASK

if [ "$ASK" = "y" ]; then
	# Umount all particions
	DRIVE_PARTS=`blkid -o device | grep $DEVICE`
	for part in $DRIVE_PARTS; do
		umount $part 2>/dev/null
	done

	# Clean drive and creating partitions
	echo "INFO: Creating partition ..."
	(echo o; echo w)  | fdisk $DEVICE				  >/dev/null 2>/dev/null # Clean drive
	(echo n; echo p; echo 1; echo; echo +2G; echo w)  | fdisk $DEVICE >/dev/null 2>/dev/null # Create partition 1G
	(echo t; echo e; echo w)  | fdisk $DEVICE			  >/dev/null 2>/dev/null # Change partition type
	(echo a; echo 1; echo w)  | fdisk $DEVICE			  >/dev/null 2>/dev/null # Make bootable

	# Device is now a partition
	DEVICE=$DEVICE"1"

	echo "INFO: Formating partition ..."
	if
	mkfs.msdos -I -n "opendomo" -F 16 $DEVICE 2>/dev/null >/dev/null
	then
		# Mount and Copy files
		echo "INFO: Copying opendomo distro ..."
		mount $DEVICE $MOUNTDIR
		cp -r $TARGETDIR/* $MOUNTDIR/

		# Wait a moment and umount
		sleep 5
		umount $DEVICE
	else
		echo "ERROR: Device $DEVICE can be mounted"
	fi
else
	exit 1
fi
