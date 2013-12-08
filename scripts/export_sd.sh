#!/bin/sh

# Variables

SCRIPTSDIR="scripts"
. $SCRIPTSDIR/sdk_variables.sh

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

# Check parted
if ! test -x /usr/bin/syslinux; then
	echo "ERROR: You need install syslinux package"
	exit 1
fi

# Clean
rm -f "$IMAGEDIR/sysconf/sysconf.tar.gz" 2>/dev/null

# Creating opendomo configuration file
mkdir -p $IMAGEDIR/sysconf
cd $FILESDIR/sysconf
tar cfp ../../$IMAGEDIR/sysconf/defconf.tar *
gzip -f ../../$IMAGEDIR/sysconf/defconf.tar
cd ../../

# Copy ISOFILES
cp -r $ISOFILESDIR/* $IMAGEDIR/

# Exporting to SD
echo "INFO: Exporting to SD ($DEVICE)..."
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

		# Installing syslinux
		if syslinux -i $DEVICE; then
			# Wait a moment, and unmount
			sleep 5
			umount $DEVICE
		else
			echo "ERROR: syslinux can be installed in SD"
		fi
	else
		echo "ERROR: Device $DEVICE can be mounted"
	fi
else
	exit 1
fi

exit 0
