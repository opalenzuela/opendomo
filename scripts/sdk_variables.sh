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
if [ "$ARCH" != "i386" ]; then
	KERNEL_VERSION="3.10.11-rt7"
	KERNEL_SOURCES="3.10"
	KERNEL_URLSRC="http://archive.raspbian.org/raspbian/pool/main/l/linux-3.10/linux-source-3.10_3.10.11-1+rpi3_all.deb"
	KERNEL_URLPKG=""
else
	KERNEL_VERSION="3.10.11-rt7"
	KERNEL_SOURCES="3.10"
	KERNEL_URLSRC="http://ftp.es.debian.org/debian/pool/main/l/linux/linux-source-3.10_3.10.11-1~bpo70+1_all.deb"
	KERNEL_URLPKG=""
fi
OD_VERSION="2.0beta1"
IMGNAME="opendomo-$OD_VERSION-$ARCH"

# Configuration and other files

ROOTSTRAPCFG="$FILESDIR/rootstrap.conf"
KERNELCFG="$FILESDIR/kernel.$ARCH.conf"
INITRDIMG="$TMPDIR/initrd.$ARCH.tar.xz"
FREESIZE="5000"

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
EXPORTDIR="exports"

# SDK commands
CHROOT="linux32 chroot"
