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
		exit 0
	fi

	# Extracting initrd
	cd $TMPDIR
	tar xfJp ../$INITRDIMG
	cd ..
  ;;
  make )
	# Creating image basic structure
	rm $IMAGEDIR/initrd 	2>/dev/null
	mkdir -p $IMAGEDIR/files/apt

	# Creating opendomo configurations
	echo 'LABEL="opendomodistro"' > $IMAGEDIR/opendomo.cfg
	echo 'CONFDEVICE="1"' >> $IMAGEDIR/opendomo.cfg
	echo 'SYSDEVICE="1"' >> $IMAGEDIR/opendomo.cfg
	echo "$OD_VERSION" > $INITRDDIR/etc/VERSION

	# Checking initrd size
	INITRDSIZE=`du $INITRDDIR | tail -n1 | cut -f1 -di`
	SIZE=`expr $INITRDSIZE + 5000`

	# Creating initrd
	if dd if=/dev/zero of=$IMAGEDIR/initrd bs=1024 count=$SIZE >/dev/null 2>/dev/null; then
		mkfs.ext2 -F $IMAGEDIR/initrd >/dev/null 2>/dev/null
		mount -o loop $IMAGEDIR/initrd $MOUNTDIR
		cp -rp $INITRDDIR/* $MOUNTDIR

		# Move debian no critical files to ISO
		mv $MOUNTDIR/var/lib/apt        $IMAGEDIR/files/apt/apt-db
		mv $MOUNTDIR/var/cache/apt      $IMAGEDIR/files/apt/apt-cache

		# Linking ISO dirs to target
		ln -s /media/opendomodistro/files/apt/apt-db    $MOUNTDIR/var/lib/apt
		ln -s /media/opendomodistro/files/apt/apt-cache $MOUNTDIR/var/cache/apt

		# Unmount initrd and compress
		umount $MOUNTDIR
		gzip $IMAGEDIR/initrd
	fi
  ;;
esac

exit 0
