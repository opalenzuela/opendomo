#!/bin/sh

# Variables
SCRIPTSDIR="scripts"
. $SCRIPTSDIR/sdk_variables.sh

# Check rootstrap
if ! test -d $ROOTSTRAPDIR; then
	echo "ERROR: You need execute make first"
	exit 1
fi

extract_kernel () {
	echo "INFO: Extracting kernel ..."
	tar xfjp $ROOTSTRAPDIR/usr/src/linux-source-"$KERNEL_SOURCES".tar.bz2
	mv linux-source-"$KERNEL_SOURCES" $KERNELDIR

	# Debian patch
	cp $ROOTSTRAPDIR/usr/src/linux-patch-"$KERNEL_SOURCES"-rt.patch.bz2 $ROOTSTRAPDIR/usr/src/linux.patch.bz2
	bunzip2 $ROOTSTRAPDIR/usr/src/linux.patch.bz2
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
			cp $KERNELDIR/arch/arm/boot/zImage $TARGETDIR/linux
		else
			echo "ERROR: Kernel isn't compiled yet"
			exit 1
		fi
	else
		if test -f $KERNELDIR/arch/x86/boot/bzImage; then
			cp $KERNELDIR/arch/x86/boot/bzImage $TARGETDIR/linux
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
