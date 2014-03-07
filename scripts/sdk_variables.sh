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
ISOFILESDIR="$FILESDIR/isofiles"
RPIFILESDIR="$FILESDIR/rpifiles"

# Opendomo packages and versions
ARCHCFG="$TMPDIR/arch"
ARCH=`cat $ARCHCFG 2>/dev/null`
ODPKG="odcommon odcgi odhal"
OD_VERSION="2.0beta2"
IMGNAME="opendomo-$OD_VERSION-$ARCH"

# Kernel version and downloads
if [ "$ARCH" != "i386" ]; then
        KERNEL_SOURCES="3.12"
        KERNEL_PACKAGE="linux-source-3.12_3.12.6-2+rpi1_all.deb"
        KERNEL_VERSION="3.12.6-rt7"
        KERNEL_URL="http://archive.raspbian.org/raspbian/pool/main/l/linux/$KERNEL_PACKAGE"
else
        KERNEL_SOURCES="3.13"
        KERNEL_PACKAGE="linux-source-3.13_3.13.4-1_all.deb"
        KERNEL_VERSION="3.13.4"
        KERNEL_URL="http://ftp.debian.org/debian/pool/main/l/linux/$KERNEL_PACKAGE"
fi

# Configuration and other files
ROOTSTRAPCFG="$FILESDIR/rootstrap.conf"
KERNELCFG="$FILESDIR/kernel.$ARCH.conf"
INITRDIMG="$TMPDIR/initrd.$ARCH.tar.xz"
FREESIZE="5000"
CHANGESIMG="dfchange.img"

# Downloads
if [ "$ARCH" != "i386" ]; then
	INITRDURL="https://www.dropbox.com/s/sk2qdrzlfj1ko9u/initrd.arm.tar.xz"
else
	INITRDURL="https://www.dropbox.com/s/8kmciuirij8ut2o/initrd.i386.tar.xz"
fi

# SDK temporal directories
INITRDDIR="$TMPDIR/initrd.$ARCH"
ROOTSTRAPDIR="$TMPDIR/rootstrap.$ARCH"
TARGETDIR="$TMPDIR/image.$ARCH"
KERNELDIR="$ROOTSTRAPDIR/usr/src/linux"
EXPORTDIR="exports"
IMAGESDIR="$TARGETDIR/images"

# SDK commands
CHROOT="linux32 chroot"
