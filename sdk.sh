#!/bin/sh
#desc:Opendomo builder based on debian.

##
## Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later
##
##Welcome to OpenDomo OS 2 SDK
##USAGE:
##
##  ./sdk.sh build i386        - Build opendomo for i386 arch
##  ./sdk.sh build arm         - Build opendomo for RaspberryPi
##
##  ./sdk.sh export sd dev     - Export opendomo distro to SD card (i386 only)
##  ./sdk.sh export vmdk       - Export opendomo distro to VMDK image (i386 only)
##  ./sdk.sh export rpi dev    - Export opendomo distro to RaspberryPI SD card
##
##  ./sdk.sh clean             - Clean images and custom configs
##
##  ./sdk.sh update            - Update the source files (do it before building)
##


# Selecting Variables

SCRIPTSDIR="scripts"
TMPDIR="tmp"
ARCHCFG="$TMPDIR/arch"

# Previous checks
if test -z $1; then
	#$SCRIPTSDIR/sdk_help.sh
	grep "^##" $0 | sed 's/##//'
	exit 1
fi

# Check user and sudo
if [ `whoami` != "root" ]; then
	echo
else
	echo "WARN: You are root, this SDK can't be executed by root"
	exit 1
fi


# Main
case $1 in
  update)
	echo "Updating files..."
	if test -d ".svn"
	then
		svn update
	else
		git pull
	fi
	
  ;;
  build )
	# Check sudo
	if ! sudo echo -n; then
		echo "ERROR: You don't have permissions to execute this SDK"
		exit 1
	fi  
  
	# Checks
	if test -z $2; then
		echo "ERROR: You need select arch to build [i386 or arm]"
		exit 1
	fi

	# Creating tmp dir
	mkdir -p $TMPDIR

	# Validating arch
	if [ $2 = i386 ] || [ $2 = arm ]; then
		echo "$2" > $ARCHCFG
	else
		echo "ERROR: arch selected is not valid"
		#$SCRIPTSDIR/sdk_help.sh
		grep "^##" $0 | sed 's/##//'
		exit 1
	fi

	# Extract sdk variables
	. $SCRIPTSDIR/sdk_variables.sh

	echo "INFO: Building OpenDomoOS for $ARCH ..."
	# Build all
	$SCRIPTSDIR/sdk_downloads.sh
	echo "  Processing Initrd (1/7) ..." 
	sudo $SCRIPTSDIR/build_initrd.sh extract || exit 1
	echo "  Processing rootstrap (2/7) ..." 
	sudo $SCRIPTSDIR/build_rootstrap.sh || exit 1
	echo "  Making packages (3/7) ..." 
	sudo $SCRIPTSDIR/build_odpkg.sh make || exit 1
	echo "  Installing packages (4/7) ..." 
	sudo $SCRIPTSDIR/build_odpkg.sh install || exit 1
	echo "  Making kernel (5/7) ..." 
	sudo $SCRIPTSDIR/build_kernel.sh make || exit 1
	echo "  Installing kernel (6/7) ..." 
	sudo $SCRIPTSDIR/build_kernel.sh install || exit 1
	echo "  Building initrd (7/7) ..." 
	sudo $SCRIPTSDIR/build_initrd.sh make || exit 1
  ;;
  export )
	# Check sudo
	if ! sudo echo -n; then
		echo "ERROR: You don't have permissions to execute this SDK"
		exit 1
	fi    
  
	# Extract sdk variables
	. $SCRIPTSDIR/sdk_variables.sh

	# Check
	if ! test -f $TARGETDIR/initrd.gz; then
		echo "ERROR: You need execute build first"
		exit 1
	fi
	if ! test -f $TARGETDIR/linux; then
		echo "ERROR: You need execute build first"
		exit 1
	fi

	# Validating image type and execute
	if test -z $2; then
		echo "ERROR: You need to select the image type"
		exit 1
	else
		if ! test -x "$SCRIPTSDIR/export_$2.sh"; then
			echo "ERROR: Image type don't exist"
			#$SCRIPTSDIR/sdk_help.sh
			grep "^##" $0 | sed 's/##//'
			exit 1
		else
			sudo $SCRIPTSDIR/export_$2.sh $3 || exit 1
		fi
	fi
  ;;
  clean )
	echo "INFO: Clean SDK ..."
	sudo $SCRIPTSDIR/sdk_clean.sh || exit 1
  ;;
  * ) 
	echo "ERROR: command selected is not valid"
	#$SCRIPTSDIR/sdk_help.sh
	grep "^##" $0 | sed 's/##//'
  ;;
esac
