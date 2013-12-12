#!/bin/sh

# Variables

SCRIPTSDIR="scripts"
. $SCRIPTSDIR/sdk_variables.sh
USER=`users | cut -f1 -d" "`


# Check
if test -z $1; then
        echo "ERROR: You need select size"
        exit 1
fi

if ! test -x "/usr/bin/qemu-img"; then
	echo "ERROR: You need install qemu first"
	exit 1
fi

# Exporting to RAW image
echo "INFO: Export opendomo to RAW image ..."
rm $EXPORTDIR/$IMGNAME.img 2>/dev/null

# Creating image
qemu-img create -f raw $EXPORTDIR/$IMGNAME.img $1
ls $EXPORTDIR

# Creating RAW image
losetup -f $EXPORTDIR/$IMGNAME.img
LOOPDEV=`losetup -a | grep -m1 "$EXPORTDIR/$IMGNAME.img" | cut -f1 -d:`

if
mkfs.vfat $LOOPDEV
then
	mount $EXPORTDIR/$IMGNAME.img $MOUNTDIR
	cp -rp $IMAGEDIR/* $MOUNTDIR/

	# Installing bootloader
	extlinux -i $MOUNTDIR

	# Wait for unmount
	sleep 5
	umount $MOUNTDIR
	losetup -d $LOOPDEV
fi

# Convert raw to vmdk
qemu-img convert -O vmdk $EXPORTDIR/$IMGNAME.img $EXPORTDIR/$IMGNAME.vmdk

# Fix image perms
chown $USER $EXPORTDIR/$IMGNAME.img
chown $USER $EXPORTDIR/$IMGNAME.vmdk
