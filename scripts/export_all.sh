#!/bin/sh

# Variables

SCRIPTSDIR="scripts"
. $SCRIPTSDIR/sdk_variables.sh

# Clean
rm -f "$IMAGEDIR/sysconf/sysconf.tar.gz"

# Force arch to i386 for all exports
# (Raspberry Pi export don't use this file)
echo "i386" >$ARCHCFG

# Creating opendomo configuration file
mkdir -p $IMAGEDIR/sysconf
cd $FILESDIR/sysconf
tar cfp ../../$IMAGEDIR/sysconf/defconf.tar * --owner 1000 --group 1000
gzip -f ../../$IMAGEDIR/sysconf/defconf.tar
cd ../../
