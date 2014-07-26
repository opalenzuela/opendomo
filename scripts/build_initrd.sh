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
	echo "INFO: Creating images ..."
	# Creating image basic structure
	rm $TARGETDIR/initrd.gz 2>/dev/null
	mkdir -p $IMAGESDIR 	2>/dev/null

	# Creating opendomo configurations
	echo 'LABEL="opendomodistro"' > $TARGETDIR/opendomo.cfg
	echo 'CONFDEVICE="1"' >> $TARGETDIR/opendomo.cfg
	echo 'SYSDEVICE="1"' >> $TARGETDIR/opendomo.cfg
	echo 'HOMEDEVICE="1"' >> $TARGETDIR/opendomo.cfg

	# Adding opendomo version
	echo "PRETTY_NAME=\"Opendomo OS version $OD_VERSION\"" 					 > $INITRDDIR/etc/os-release
	echo "NAME=\"Opendomo OS\""								>> $INITRDDIR/etc/os-release
 	echo "VERSION_ID=$OD_VERSION"								>> $INITRDDIR/etc/os-release
     	echo "VERSION=$OD_VERSION"								>> $INITRDDIR/etc/os-release
    	echo "ID=opendomo"									>> $INITRDDIR/etc/os-release
   	echo "ANSI_COLOR=\"1;31\""								>> $INITRDDIR/etc/os-release
  	echo "HOME_URL=http://opendomo.org/"							>> $INITRDDIR/etc/os-release
 	echo "SUPPORT_URL=http://www.opendomo.com/wiki/index.php?title=P%C3%A1gina_Principal"	>> $INITRDDIR/etc/os-release
	echo "BUG_REPORT_URL=https://github.com/opalenzuela/opendomo/issues"			>> $INITRDDIR/etc/os-release
	echo "$OD_VERSION"									>> $INITRDDIR/etc/VERSION

	# Creating raw image files
	if ! test -f $IMAGESDIR/$DEFCHANGESIMG; then
		if dd if=/dev/zero of=$IMAGESDIR/$DEFCHANGESIMG bs=1024 count=1500000 >/dev/null 2>/dev/null; then

			# Creating fs and copy files
			mkfs.ext2 -F $IMAGESDIR/$DEFCHANGESIMG >/dev/null >/dev/null 2>/dev/null
			mount -o loop $IMAGESDIR/$DEFCHANGESIMG $MOUNTDIR 2>/dev/null
			mkdir -p $MOUNTDIR/usr && mv $INITRDDIR/usr/share $MOUNTDIR/usr/ 2>/dev/null

			# Unmount
			while !	umount $MOUNTDIR 2>/dev/null; do
				sleep 1
			done

			# Creating default changes and custom changes image
			cp $IMAGESDIR/$DEFCHANGESIMG $IMAGESDIR/$CSTCHANGESIMG
			gzip $IMAGESDIR/$DEFCHANGESIMG

			# Creating home image
			if ! test -f $IMAGESDIR/$HOMEFSIMG; then
				dd if=/dev/zero of=$IMAGESDIR/$HOMEFSIMG bs=1024 count=10000 2>/dev/null
				mkfs.ext2 -F $IMAGESDIR/$HOMEFSIMG >/dev/null 2>/dev/null
        		fi
		fi
	fi

	# Checking initrd size
	INITRDSIZE=`du $INITRDDIR | tail -n1 | sed 's/\t.*//'`
	SIZE=`expr $INITRDSIZE + $FREESIZE`

	if [ "$ARCH" != "i386" ]; then
		# Clean emulator for RaspberryPi
		rm $INITRDDIR/usr/bin/qemu-arm-static 2>/dev/null

		# Copy RasberryPi firmware to boot
		cp $RPIFILESDIR/bootcode.bin $INITRDDIR/boot/
		cp $RPIFILESDIR/start* $INITRDDIR/boot/
		cp $RPIFILESDIR/fixup* $INITRDDIR/boot/

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
		chown -R 1000:1000 $MOUNTDIR/home/admin

		# Unmount initrd and compress
		while !	umount $MOUNTDIR 2>/dev/null; do
			sleep 1
		done
		gzip $TARGETDIR/initrd
	fi
  ;;
esac

exit 0
