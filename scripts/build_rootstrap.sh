#!/bin/sh

# Variables

SCRIPTSDIR="scripts"
. $SCRIPTSDIR/sdk_variables.sh
PACKAGES=`cat $ROOTSTRAPCFG`

# Check and clean
if ! test -f $INITRDIMG; then
        echo "ERROR: initrd image don't exist"
        exit 1
fi
if test -d $ROOTSTRAPDIR; then
	echo "WARN: rootstrap is already created"
	exit 0
fi

# Copy initrd to create rootstrap
cp -rp $INITRDDIR $ROOTSTRAPDIR

# Installing qemu for RaspberryPi
if [ "$ARCH" != "i386" ]; then
	if
	cp /usr/bin/qemu-arm-static $ROOTSTRAPDIR/usr/bin
	then
		echo "INFO: Installing emulator in RaspberryPi rootstrap ..."
	else
		echo "ERROR: You need qemu-arm-static to create RaspberryPi distro"
	fi
fi

# Update rootstrap apt sources
cp /etc/resolv.conf $ROOTSTRAPDIR/etc/
chroot "$ROOTSTRAPDIR" /bin/bash -c "apt-get update"

# Installing rootstrap packages
chroot "$ROOTSTRAPDIR" /bin/bash -c "DEBIAN_FRONTEND=noninteractive apt-get --force-yes -yq install $PACKAGES"
chroot "$ROOTSTRAPDIR" /bin/bash -c "dpkg-reconfigure --frontend=noninteractive debconf"
chroot "$ROOTSTRAPDIR" /bin/bash -c "apt-get clean"
