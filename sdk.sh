#!/bin/sh
#desc:Opendomo builder based on debian.

# Selecting Variables

SCRIPTSDIR="scripts"
ARCHCFG="tmp/arch"

# Check user and sudo
if [ `whoami` != "root" ]; then
	if ! sudo echo -n; then
		echo "ERROR: You don't have permissions to execute this SDK"
		exit 1
	fi
else
	echo "WARN: You are root, this SDK can't be executed by root"
	exit 1
fi

# Previous checks
if test -z $1; then
	$SCRIPTSDIR/sdk_help.sh
	exit 1
fi

# Main
case $1 in
  build )
	# Checks
	if test -z $2; then
		echo "ERROR: You need select arch to build [i386 or arm]"
		exit 1
	fi

	# Validating arch
	if [ $2 = i386 ] || [ $2 = arm ]; then
		echo "$2" > $ARCHCFG
	else
		echo "ERROR: arch selected is not valid"
		$SCRIPTSDIR/sdk_help.sh
		exit 1
	fi

	# Extract sdk variables
	. $SCRIPTSDIR/sdk_variables.sh

	echo "INFO: Build opendomo for $ARCH"
	# Build all
	$SCRIPTSDIR/sdk_downloads.sh
	sudo $SCRIPTSDIR/build_initrd.sh extract
	sudo $SCRIPTSDIR/build_rootstrap.sh
	sudo $SCRIPTSDIR/build_odpkg.sh make
	sudo $SCRIPTSDIR/build_odpkg.sh install
	sudo $SCRIPTSDIR/build_kernel.sh make
	sudo $SCRIPTSDIR/build_kernel.sh install
	sudo $SCRIPTSDIR/build_initrd.sh make
  ;;
  export )
	# Extract sdk variables
	. $SCRIPTSDIR/sdk_variables.sh

	# Check
	if ! test -f $IMAGEDIR/initrd.gz; then
		echo "ERROR: You need execute build first"
		exit 1
	fi
	if ! test -f $IMAGEDIR/linux; then
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
			$SCRIPTSDIR/sdk_help.sh
			exit 1
		else
			sudo $SCRIPTSDIR/export_$2.sh $3
		fi
	fi
  ;;
  clean )
	echo "INFO: Clean SDK ..."
	sudo $SCRIPTSDIR/sdk_clean.sh
  ;;
  * )
	echo "ERROR: command selected is not valid"
	$SCRIPTSDIR/sdk_help.sh
  ;;
esac

