#!/bin/sh

# Variables
SCRIPTSDIR="scripts"
. $SCRIPTSDIR/sdk_variables.sh

# Creating emply folders, git don't work with emply folders
mkdir -p $TMPDIR
mkdir -p $MOUNTDIR
mkdir -p $IMAGEDIR
mkdir -p $EXPORTDIR

# Download initrd
if ! test -f $INITRDIMG; then
	cd $TMPDIR
	wget $INITRDURL
fi
