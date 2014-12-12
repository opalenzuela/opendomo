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

if test -z "$1"; then
    grep "^##" $0 | sed 's/##//'
    exit 1

elif [ "$1" = "update" ]; then
    # Previous checks
    if [ "$1" = "update" ] && ! test -x /usr/bin/git; then
        echo -e "[${ERRO} err ${NORL}] You need install git to update SDK" & exit 1
    fi

    echo -e "########## Cleaning OpenDomoOS SDK ##########"
    echo
    echo -ne "[${INFO} 1/1 ${NORL}] Cleaning SDK ...                             "
    if git pull &>/dev/null; then
        echo -e "(${DONE}done${NORL})"
    else
        echo -e "(${ERRO}failed${NORL})"
        exit 1
    fi
    echo
else
    sudo $FUNCDIR/fct_main $1 $2 $3
fi
