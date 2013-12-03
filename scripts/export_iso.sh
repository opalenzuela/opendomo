#!/bin/sh

# Variables

SCRIPTSDIR="scripts"
. $SCRIPTSDIR/sdk_variables.sh

echo "INFO: Export opendomo in ISO"

# Clean
rm -f "$IMGNAME.iso" 2>/dev/null
rm -f "$IMAGEDIR/sysconf/sysconf.tar.gz"

# Creating opendomo configuration file
mkdir -p $IMAGEDIR/sysconf
cd $FILESDIR/sysconf
tar cfp ../../$IMAGEDIR/sysconf/sysconf.tar *
gzip ../../$IMAGEDIR/sysconf/sysconf.tar
cd ../../

# Copy ISOFILES
cp -r $ISOFILESDIR/* $IMAGEDIR/

# Generating ISO
if
genisoimage -o "$IMGNAME.iso" -no-emul-boot -b isolinux.bin -c boot.cat -boot-load-size 4 -boot-info-table -J -input-charset utf-8 -hide-rr-moved -R $IMAGEDIR >/dev/null 2>/dev/null
then
	echo "INFO: Generating ISO ..."
else
	echo "ERROR: genisoimage can't create image, check your configuration"
fi
