#!/bin/sh

# Variables
SCRIPTSDIR="scripts"
. $SCRIPTSDIR/sdk_variables.sh

# Check rootstrap
if ! test -d $ROOTSTRAPDIR; then
	echo "ERROR: You need execute make first"
	exit 1
fi

download_kernel () {
	KERNELPKG=`basename $KERNEL_URLSRC`
	if ! test -f $TMPDIR/$KERNELPKG; then
		echo "INFO: Downloading kernel ..."
		cd $TMPDIR
		wget $KERNEL_URLSRC
		cd ..

		echo "INFO: Installing kernel ..."
		cp $TMPDIR/$KERNELPKG $ROOTSTRAPDIR/tmp
	        $CHROOT "$ROOTSTRAPDIR" /bin/bash -c "cd tmp && dpkg -i $KERNELPKG"
	fi
}

extract_kernel () {
	echo "INFO: Extracting kernel ..."
	tar xfJp $ROOTSTRAPDIR/usr/src/linux-source-"$KERNEL_SOURCES".tar.xz
	mv linux-source-"$KERNEL_SOURCES" $KERNELDIR

	# Debian patch
	cp $ROOTSTRAPDIR/usr/src/linux-patch-"$KERNEL_SOURCES"-rt.patch.xz $ROOTSTRAPDIR/usr/src/linux.patch.xz
	xz --decompress $ROOTSTRAPDIR/usr/src/linux.patch.xz
	mv $ROOTSTRAPDIR/usr/src/linux.patch $KERNELDIR
	$CHROOT "$ROOTSTRAPDIR" /bin/bash -c "cd /usr/src/linux && patch --quiet -p1 <linux.patch 2>/dev/null"
	rm $KERNELDIR/linux.patch

	# Copy kernel config
	cp "$KERNELCFG" "$KERNELDIR"/.config
}

case $1 in
  make )

	# Extract kernel
	if ! test -d $KERNELDIR; then
		download_kernel
		extract_kernel
	else
		echo "WARN: kernel is already extracted"
	fi

	KERNELDIR="/usr/src/linux"
	$CHROOT "$ROOTSTRAPDIR" /bin/bash -c "cd $KERNELDIR && make"
  ;;
  install )
	# Install compiled kernel
	if [ "$ARCH" = "arm" ]; then
		if test -f $KERNELDIR/arch/arm/boot/zImage; then
			cp $KERNELDIR/arch/arm/boot/zImage $IMAGEDIR/linux
		else
			echo "ERROR: Kernel isn't compiled yet"
			exit 1
		fi
	else
		if test -f $KERNELDIR/arch/x86/boot/bzImage; then
			cp $KERNELDIR/arch/x86/boot/bzImage $IMAGEDIR/linux
		else
			echo "ERROR: Kernel isn't compiled yet"
			exit 1
		fi
	fi

	# Clean old, and install modules
	rm -r $INITRDDIR/lib/modules/ 2>/dev/null
	DIR=`pwd`

	cd $KERNELDIR
	for module in `find ./ -name *.ko`; do
		MODULEDIR=`dirname $module`
		mkdir -p $DIR/$INITRDDIR/lib/modules/$KERNEL_VERSION/$MODULEDIR
		cp -r $module $DIR/$INITRDDIR/lib/modules/$KERNEL_VERSION/$MODULEDIR
	done
	cd $DIR

	# Copy modules db
	cp $KERNELDIR/modules.order $INITRDDIR/lib/modules/$KERNEL_VERSION
	cp $KERNELDIR/modules.builtin $INITRDDIR/lib/modules/$KERNEL_VERSION
	$CHROOT "$INITRDDIR" /bin/bash -c "/sbin/depmod -a $KERNEL_VERSION"
  ;;
  configure )
	# Extract kernel
	if ! test -d $KERNELDIR; then
		extract_kernel
	else
		echo "WARN: kernel is already extracted"
	fi

	KERNELDIR="/usr/src/linux"
        $CHROOT "$ROOTSTRAPDIR" /bin/bash -c "cd $KERNELDIR && make menuconfig"
  ;;
esac

exit 0
