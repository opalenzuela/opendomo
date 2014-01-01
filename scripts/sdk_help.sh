#!/bin/sh
# desc: Help for opendomo SDK

cat <<- HELPEND
	Welcome debian opendomo SDK
	USAGE:

	   ./sdk.sh build i386        - Build opendomo for i386 arch
	   ./sdk.sh build arm         - Build opendomo for RaspberryPi

	   ./sdk.sh export sd dev     - Export opendomo distro to SD card (i386 only)"
	   ./sdk.sh export vmdk size  - Export opendomo distro to VMDK image (i386 only)"
	   ./sdk.sh export rpi dev    - Export opendomo distro to RaspberryPI SD card"

	   ./sdk.sh clean             - Clean images and custom configs"

HELPEND
