#!/bin/sh

# Variables

SCRIPTSDIR="scripts"
. $SCRIPTSDIR/sdk_variables.sh

# Clean
rm -f "$IMAGEDIR/sysconf/sysconf.tar.gz"

# Force arch to i386 for all exports
# (Raspberry Pi export don't use this file)
echo "i386" >$ARCHCFG

# Copy package default config
rm -r $FILESDIR/sysconf 2>/dev/null
mkdir -p $FILESDIR/sysconf/etc/opendomo

for config in $ODPKG; do
	cp -r $SRCDIR/$pkg/etc/opendomo/* $FILESDIR/sysconf/etc/opendomo/* 2>/dev/null
done

# Creating opendomo configuration file
mkdir -p $IMAGEDIR/sysconf
cd $FILESDIR/sysconf
tar cfp ../../$IMAGEDIR/sysconf/defconf.tar * --owner 1000 --group 1000
gzip -f ../../$IMAGEDIR/sysconf/defconf.tar
cd ../../
