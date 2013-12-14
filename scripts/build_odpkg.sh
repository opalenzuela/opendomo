#!/bin/sh

# Variables

SCRIPTSDIR="scripts"
. $SCRIPTSDIR/sdk_variables.sh

# Checks and clean
if ! test -d $ROOTSTRAPDIR; then
        echo "ERROR: rootstrap don't exist"
        exit 1
fi

# Copy packages into rootstrap

case $1 in
  make )
	# Clean old packages
	rm -r $ROOTSTRAPDIR/tmp/* 2>/dev/null

	echo -n "INFO: Creating debian package:"
	for pkg in $ODPKG; do
		echo -n " $pkg"

		# Copy package into rootstrap and build
		cp -r $SRCDIR/$pkg $ROOTSTRAPDIR/tmp/ 2>/dev/null
		$CHROOT "$ROOTSTRAPDIR" /bin/bash -c "cd /tmp/$pkg && dpkg-buildpackage 2>/dev/null >/dev/null"
	done
	echo
  ;;
  install )
	echo "INFO: Installing package in initrd ..."
	cp $ROOTSTRAPDIR/tmp/*.deb $INITRDDIR/tmp/
	$CHROOT "$INITRDDIR" /bin/bash -c "dpkg -i /tmp/*.deb"
  ;;
esac

exit 0
