#!/bin/sh

# Variables

SCRIPTSDIR="scripts"
. $SCRIPTSDIR/sdk_variables.sh


# Clean
rm -f "$EXPORTDIR/$IMGNAME.iso" 2>/dev/null

# Copy ISOFILES
cp -r $ISOFILESDIR/* $IMAGEDIR/

# Generating ISO
if
genisoimage -o "$EXPORTDIR/$IMGNAME.iso" -no-emul-boot -b isolinux.bin -c boot.cat -boot-load-size 4 -boot-info-table -J -input-charset utf-8 -hide-rr-moved -R $IMAGEDIR >/dev/null 2>/dev/null
then
	echo "INFO: Export opendomo to ISO file ..."
else
	echo "ERROR: genisoimage can't create image, check your configuration"
fi
