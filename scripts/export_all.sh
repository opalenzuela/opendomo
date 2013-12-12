#!/bin/sh

# Variables

SCRIPTSDIR="scripts"
. $SCRIPTSDIR/sdk_variables.sh

# Clean
rm -f "$IMAGEDIR/sysconf/sysconf.tar.gz"

# Creating opendomo configuration file
mkdir -p $IMAGEDIR/sysconf
cd $FILESDIR/sysconf
tar cfp ../../$IMAGEDIR/sysconf/defconf.tar *
gzip -f ../../$IMAGEDIR/sysconf/defconf.tar
cd ../../

# Copy ISOFILES
cp -r $ISOFILESDIR/* $IMAGEDIR/
