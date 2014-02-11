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
	rm $TARGETDIR/initrd.gz 2>/dev/null
	mkdir -p $IMAGESDIR 	2>/dev/null

	# Creating opendomo configurations
	echo 'LABEL="opendomodistro"' > $TARGETDIR/opendomo.cfg
	echo 'CONFDEVICE="1"' >> $TARGETDIR/opendomo.cfg
	echo 'SYSDEVICE="1"' >> $TARGETDIR/opendomo.cfg
	echo 'HOMEDEVICE="1"' >> $TARGETDIR/opendomo.cfg
	echo "$OD_VERSION" > $INITRDDIR/etc/VERSION

	# Creating raw file to move no critical files
	if ! test -f $IMAGESDIR/$CHANGESIMG; then
		if dd if=/dev/zero of=$IMAGESDIR/$CHANGESIMG bs=1024 count=400000 >/dev/null 2>/dev/null; then

			# Creating fs and copy files
			mkfs.ext2 -F $IMAGESDIR/$CHANGESIMG >/dev/null 2>/dev/null
			mount -o loop $IMAGESDIR/$CHANGESIMG $MOUNTDIR 2>/dev/null
			mkdir -p $MOUNTDIR/usr && mv $INITRDDIR/usr/share $MOUNTDIR/usr/ 2>/dev/null

			# Move apt files to SD and create links in initrd
			mkdir $TARGETDIR/apt
			mv $INITRDDIR/var/lib/apt   $TARGETDIR/apt/lib   2>/dev/null
			mv $INITRDDIR/var/cache/apt $TARGETDIR/apt/cache 2>/dev/null
			ln -s /mnt/odconf/apt/lib   $INITRDDIR/var/lib/apt   2>/dev/null
			ln -s /mnt/odconf/apt/cache $INITRDDIR/var/cache/apt 2>/dev/null

			# Unmount
			while !	umount $MOUNTDIR 2>/dev/null; do
				sleep 1
			done
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
	if dd if=/dev/zero of=$TARGETDIR/initrd bs=1024 count=$SIZE >/dev/null 2>/dev/null; then
		mkfs.ext2 -F $TARGETDIR/initrd >/dev/null 2>/dev/null
		mount -o loop $TARGETDIR/initrd $MOUNTDIR
		cp -rp $INITRDDIR/* $MOUNTDIR

		# Force home directories permissions
		chmod 700 $MOUNTDIR/home/admin
		chmod 700 $MOUNTDIR/home/user
		chown -R 1000:1000 $MOUNTDIR/home/admin
		chown -R 1001:100  $MOUNTDIR/home/user

		# Unmount initrd and compress
		while !	umount $MOUNTDIR 2>/dev/null; do
			sleep 1
		done
		gzip $TARGETDIR/initrd
	fi
  ;;
esac

exit 0
