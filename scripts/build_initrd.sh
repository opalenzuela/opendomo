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

	# Creating raw file to move no critical files
	if ! test -f $IMAGEDIR/$CHANGESIMG.gz; then
		if dd if=/dev/zero of=$IMAGEDIR/$CHANGESIMG bs=1024 count=500000 >/dev/null 2>/dev/null; then

			# Creating fs and copy files
			mkfs.ext2 -F $IMAGEDIR/$CHANGESIMG >/dev/null 2>/dev/null
			mount -o loop $IMAGEDIR/$CHANGESIMG $MOUNTDIR 2>/dev/null
			mkdir -p $MOUNTDIR/usr && mv $INITRDDIR/usr/share $MOUNTDIR/usr/ 2>/dev/null

			# Unmount and compress image, in first boot will be decompressed
			while !	umount $MOUNTDIR 2>/dev/null; do
				sleep 1
			done
			gzip $IMAGEDIR/$CHANGESIMG 2>/dev/null
		fi
	fi

	# Checking initrd size
	INITRDSIZE=`du $INITRDDIR | tail -n1 | sed 's/\t.*//'`
	SIZE=`expr $INITRDSIZE + $FREESIZE`

	if [ "$ARCH" != "i386" ]; then
		# Clean emulator for RaspberryPi
		rm $INITRDDIR/usr/bin/qemu-arm-static 2>/dev/null

		# Creating RaspberryPi boot config file
		echo "rw root=/dev/ram0 ramdisk_size=$SIZE quiet rootwait" >$RPIFILESDIR/cmdline.txt

	else
		# Creating syslinux boot configuration files
		echo "DEFAULT linux initrd=initrd.gz ramdisk_size=$SIZE rw root=/dev/ram0 quiet" >$ISOFILESDIR/syslinux.cfg
	fi

	# Creating initrd
	if dd if=/dev/zero of=$IMAGEDIR/initrd bs=1024 count=$SIZE >/dev/null 2>/dev/null; then
		mkfs.ext2 -F $IMAGEDIR/initrd >/dev/null 2>/dev/null
		mount -o loop $IMAGEDIR/initrd $MOUNTDIR
		cp -rp $INITRDDIR/* $MOUNTDIR

		# Move apt files to SD
		mv $MOUNTDIR/var/lib/apt   $IMAGEDIR/files/apt/apt-db 2>/dev/null
		mv $MOUNTDIR/var/cache/apt $IMAGEDIR/files/apt/apt-cache 2>/dev/null

		# Unmount initrd and compress
		while !	umount $MOUNTDIR 2>/dev/null; do
			sleep 1
		done
		gzip $IMAGEDIR/initrd
	fi
  ;;
esac

exit 0
