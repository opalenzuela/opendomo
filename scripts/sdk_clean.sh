#!/bin/sh

# Variables
SCRIPTSDIR="scripts"
. $SCRIPTSDIR/sdk_variables.sh

# Clean files
if [ "$2" = "downloads" ]; then
	rm -r $TMPDIR/*.tar.xz
fi

rm $TMPDIR/arch
rm -r $TMPDIR/initrd.i386 2>/dev/null
rm -r $TMPDIR/rootstrap.i386 2>/dev/null
rm -r $TMPDIR/image.i386 2>/dev/null
rm -r $TMPDIR/initrd.arm 2>/dev/null
rm -r $TMPDIR/rootstrap.arm 2>/dev/null
rm -r $TMPDIR/image.arm 2>/dev/null

