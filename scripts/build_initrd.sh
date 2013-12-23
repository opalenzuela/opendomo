#!/bin/sh
#desc: Extract or compress initrd

# Variables

SCRIPTSDIR="scripts"
. $SCRIPTSDIR/sdk_variables.sh

# Check
if ! test -f $INITRDIMG; then
	echo "ERROR: initrd image don't exist"
	exit 1
fi

case $1 in
  extract )
	if test -d $INITRDDIR; then
		echo "WARN: initrd is already extracted"
	else
		# Extracting initrd
		cd $TMPDIR
		tar xfJp ../$INITRDIMG
		cd ..
	fi

	# Installing qemu for RaspberryPi
	if [ "$ARCH" != "i386" ]; then
		if
		cp /usr/bin/qemu-arm-static $INITRDDIR/usr/bin
	        then
			echo "INFO: Installing RaspberryPi emulator ..."
		else
			echo "ERROR: You need qemu-arm-static to create RaspberryPi distro"
			exit 1
		fi
	fi
  ;;
  make )
	# Creating image basic structure
	rm $IMAGEDIR/initrd.gz 	2>/dev/null
	mkdir -p $IMAGEDIR/files/apt

	# Creating opendomo configurations
	echo 'LABEL="opendomodistro"' > $IMAGEDIR/opendomo.cfg
	echo 'CONFDEVICE="1"' >> $IMAGEDIR/opendomo.cfg
	echo 'SYSDEVICE="1"' >> $IMAGEDIR/opendomo.cfg
	echo "$OD_VERSION" > $INITRDDIR/etc/VERSION

	# Checking initrd size
	INITRDSIZE=`du $INITRDDIR | tail -n1 | sed 's/\t.*//'`
	SIZE=`expr $INITRDSIZE + $FREESIZE`

	# Clean emulator
	rm $INITRDDIR/usr/bin/qemu-arm-static 2>/dev/null

	# Creating initrd
	if dd if=/dev/zero of=$IMAGEDIR/initrd bs=1024 count=$SIZE >/dev/null 2>/dev/null; then
		mkfs.ext2 -F $IMAGEDIR/initrd >/dev/null 2>/dev/null
		mount -o loop $IMAGEDIR/initrd $MOUNTDIR
		cp -rp $INITRDDIR/* $MOUNTDIR

		# Move debian no critical files and linking dirs
		mv $MOUNTDIR/var/lib/apt   $IMAGEDIR/files/apt/apt-db 2>/dev/null
		ln -s /mnt/odconf/files/apt/apt-db    $MOUNTDIR/var/lib/apt

		mv $MOUNTDIR/var/cache/apt $IMAGEDIR/files/apt/apt-cache 2>/dev/null
		ln -s /mnt/odconf/files/apt/apt-cache $MOUNTDIR/var/cache/apt

		# Unmount initrd and compress
		umount $MOUNTDIR
		gzip $IMAGEDIR/initrd
	fi
  ;;
esac

exit 0
