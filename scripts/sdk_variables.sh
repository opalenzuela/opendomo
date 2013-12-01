#!/bin/sh
#desc: Variables to use in SDK"

# SDK basic directories

TMPDIR="tmp"
SCRIPTSDIR="scripts"
FILESDIR="files"
MOUNTDIR="mnt"
DOCSDIR="docs"
SRCDIR="src"
TESTDIR="test"
ISOFILESDIR="$ISODIR/isofiles"

# Opendomo packages and versions

ARCHCFG="$TMPDIR/arch"
ARCH=`cat $ARCHCFG 2>/dev/null`
ODPKG="libcoelacanth odcommon odcgi odhal"
KERNEL_VERSION="3.2.51-rt72"
KERNEL_SOURCES="3.2"
OD_VERSION="1.9.4"
IMGNAME="opendomo-$OD_VERSION-$ARCH"

# Configuration and other files
ROOTSTRAPCFG="$FILESDIR/rootstrap.conf"
KERNELCFG="$FILESDIR/kernel.$ARCH.conf"
INITRDIMG="$TMPDIR/initrd.$ARCH.tar.xz"

# Downloads
if [ "$ARCH" != "i386" ]; then
	INITRDURL="https://www.dropbox.com/s/sk2qdrzlfj1ko9u/initrd.arm.tar.xz"
else
	INITRDURL="https://www.dropbox.com/s/8kmciuirij8ut2o/initrd.i386.tar.xz"
fi

# SDK temporal directories

INITRDDIR="$TMPDIR/initrd.$ARCH"
ROOTSTRAPDIR="$TMPDIR/rootstrap.$ARCH"
IMAGEDIR="$TMPDIR/image.$ARCH"
KERNELDIR="$ROOTSTRAPDIR/usr/src/linux"
