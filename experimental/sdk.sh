#!/bin/bash
#desc:Opendomo builder based on debian.

##
## Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later
##
## Welcome to OpenDomo OS 2 SDK
## USAGE:
##
##  ./sdk.sh clean             - Clean images and custom configs
##  ./sdk.sh update            - Update the source files (do it before building)
##
##  ./sdk.sh build i386        - Build opendomo for i386 arch
##  ./sdk.sh build arm         - Build opendomo for RaspberryPi
##
##  ./sdk.sh assemble i386     - Assemble opendomo for i386 arch (using downloaded files)
##  ./sdk.sh assemble arm      - Assemble opendomo for RaspberryPi (using downloaded files)
##
##  ./sdk.sh export sd dev     - Export opendomo distro to SD card (i386 only)
##  ./sdk.sh export vmdk       - Export opendomo distro to VMDK image (i386 only)
##  ./sdk.sh export rpi dev    - Export opendomo distro to RaspberryPI SD card
##
##

FUNCDIR=files/scripts/
source $FUNCDIR/fct_vars

# All options and programs to execute SDK
if test -z "$1"; then
    grep "^##" $0 | sed 's/##//'
    exit 1
elif [ "$1" = "update" ] && test -x /usr/bin/git; then
    echo -e "[${ERRO} ERROR ${NORL}] $pkg You need install git to update SDK"
    exit 1
elif [ "$1" = "build" ] && test -z $2; then
    echo -e "[${ERRO} ERROR ${NORL}] $pkg You need select a valid arch [ i386 or arm ]"
    exit 1
elif [ "$1" = "build" ] && [ "$2" != "arm" ] && [ "$2" != "i386" ]; then
    echo -e "[${ERRO} ERROR ${NORL}] $pkg Selected arch is not valid [ i386 or arm ]"
    exit 1
elif [ "$1" = "assemble" ] && test -z $2; then
    echo -e "[${ERRO} ERROR ${NORL}] $pkg You need select a valid arch [ i386 or arm ]"
    exit 1
elif [ "$1" = "assemble" ] && [ "$2" != "arm" ] && [ "$2" != "i386" ]; then
    echo -e "[${ERRO} ERROR ${NORL}] $pkg Selected arch is not valid"
    grep "^##" $0 | sed 's/##//'
    exit 1
elif [ "$1" = "export" ] && test -z $2; then
    echo -e "[${ERRO} ERROR ${NORL}] $pkg You need select a valid export [ vmdk, sd or rpi ]"
    exit 1
elif [ "$1" = "export" ] && [ "$2" != "sd" ] && [ "$2" != "vmdk" ] && [ "$2" != "rpi" ]; then
    echo -e "[${ERRO} ERROR ${NORL}] $pkg Selected option is not valid"
    grep "^##" $0 | sed 's/##//'
    exit 1
elif [ "$2" = "rpi" ] && test -z $3; then
    echo -e "[${ERRO} ERROR ${NORL}] $pkg You need select disk to export [ sda, sdb, sdc ]"
    exit 1
elif [ "$2" = "sd" ] && test -z $3; then
    echo -e "[${ERRO} ERROR ${NORL}] $pkg You need select disk to export [ sda, sdb, sdc ]"
    exit 1
fi

if [ "$1" = "update" ]; then
    $FUNCDIR_fh_sdk update
else
    sudo $FUNCDIR/fct_main $1 $2 $3
fi
