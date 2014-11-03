#!/bin/bash
# desc: Create multistrap based debian/raspian filesystems

# help
help() {
    echo
    echo "help for opendomo image creator:"
    echo "USAGE: $basename $0 --arch=\$arch build or make"
    echo
    echo "   --arch=\$arch       Arch to use, i386 or arm"
    echo
    echo "     build            Build multistrap debian or raspian image"
    echo "     make             Configure and make package for opendomo base"
    echo
}

# Colors and visual effects
ERRO='\e[0;31m'; INFO='\e[0;32m' ; WARN='\e[0;33m'; NORL='\e[0m'

# Checking options
if [ "$1" != "--arch=i386" ] && [ "$1" != "--arch=arm" ]; then
    help
    exit 1
else
    DEVICE=`echo "$1" | cut -f2 -d=`
    CONFIG=multistrap.$DEVICE.conf
    TARGET=rootfs.$DEVICE
fi
if   [ "$2" != "build" ] && [ "$2" != "make" ]; then
    help
    exit 1

elif [ `whoami` != "root" ]; then
    echo -e "[${ERRO} ERROR ${NORL}] You need execute by root or with sudo"
    exit 1

elif [ "$2" = "build" ] && test -d $TARGET; then
    echo -e "[${WARN} WARN  ${NORL}] Deleting previous images ..."
    rm -r $TARGET $TARGET.tar $TARGET.tar.gz   2>/dev/null

elif [ "$2" = "make"  ] && ! test -d $TARGET; then
    echo -e "[${ERRO} ERROR ${NORL}] You need execute build option first"
    exit 1
fi
if [ $DEVICE = arm ] && ! test -x /usr/bin/qemu-arm-static; then
    echo -e "[${ERRO} ERROR ${NORL}] You need install qemu-user-static in the system"
    exit 1
fi
if ! test -x $MULTISTRAP; then
    echo -e "[${ERRO} ERROR ${NORL}] You need install multistrap"
    exit 1
fi
if test -x /usr/bin/linux32; then
    CHROOT="linux32 chroot"
else
    CHROOT="chroot"
fi

# Variables
OD_VERSION="2.2"
CONFIGSDIR="configs"
MULTISTRAP="/usr/sbin/multistrap"
ALLPACKAGES="busybox isc-dhcp-client net-tools ifupdown openssh-server libsqlite3-0 sudo libjpeg8 libconfig9 usbutils \
             psmisc rsync ntpdate resolvconf module-init-tools aptitude wget ntp linux-base lighttpd dialog klibc-utils cpio"
ARMPACKAGES="raspberrypi-bootloader-nokernel libraspberrypi0"

if [ $DEVICE = arm ]; then
    KERNLIMGLURL="https://www.dropbox.com/s/3yyj5umxrlu517m/linux-image-3.12-1-rpi_3.12.9-1%2Brpi1_armhf.deb"
    INITRAMFSURL="http://archive.raspbian.org/raspbian/pool/main/i/initramfs-tools/initramfs-tools_0.115~bpo70%2b1_all.deb"
else
    KERNLIMGLURL="https://www.dropbox.com/s/ez1ddgpeg0971xt/linux-image-3.12-0.bpo.1-486_3.12.9-1~bpo70%2B1_i386.deb"
    INITRAMFSURL="http://ftp.debian.org/debian/pool/main/i/initramfs-tools/initramfs-tools_0.115~bpo70+1_all.deb"
fi

configure_all() {
    # Configure basic files
    touch $TARGET/etc/fstab

    echo "" 					 	 	 > $TARGET/etc/motd
    echo "Welcome to opendomo: Security and free domotics"	>> $TARGET/etc/motd
    echo ""						 	>> $TARGET/etc/motd

    rm $TARGET/bin/sh 2>/dev/null
    ln -s /bin/bash $TARGET/bin/sh
    rm $TARGET/lib64 2>/dev/null

    # Configure basic network
    echo "auto lo"			 >$TARGET/etc/network/interfaces
    echo "iface lo inet loopback"	>>$TARGET/etc/network/interfaces
    echo ""				>>$TARGET/etc/network/interfaces
    echo "auto eth0"			>>$TARGET/etc/network/interfaces
    echo "iface eth0 inet dhcp"		>>$TARGET/etc/network/interfaces
    echo ""				>>$TARGET/etc/network/interfaces
    echo "auto eth0:1"			>>$TARGET/etc/network/interfaces
    echo "allow-hotplug eth0:1"		>>$TARGET/etc/network/interfaces
    echo "iface eth0:1 inet static"	>>$TARGET/etc/network/interfaces
    echo "   address 169.254.0.25 "	>>$TARGET/etc/network/interfaces
    echo "   netmask 255.255.255.0"	>>$TARGET/etc/network/interfaces

    # Creating groups and users
    $CHROOT "$TARGET" /bin/bash -c "addgroup --gid 1000 admin >/dev/null 2>/dev/null"
    $CHROOT "$TARGET" /bin/bash -c "useradd -G disk,audio,video,plugdev -p opendomo -g 1000 -u 1000 admin 2>/dev/null"
    $CHROOT "$TARGET" /bin/bash -c "echo -e 'opendomo\nopendomo' | (passwd admin) 2>/dev/null"
    $CHROOT "$TARGET" /bin/bash -c "echo -e 'opendomo\nopendomo' | (passwd root)  2>/dev/null"

    # Creating admin folder
    mkdir -p $TARGET/home/admin
    cp $TARGET/etc/skel/.* $TARGET/home/admin/ 2>/dev/null
    chown -R 1000:1000 $TARGET/home/admin

    # Copy sshd configuration (no root access allow)
    cp $CONFIGSDIR/sshd_config $TARGET/etc/ssh/sshd_config
    # Copy new profile file (added opendomo folder to $PATH)
    cp $CONFIGSDIR/profile $TARGET/etc/profile
    # Copy new host file (Used in sudo)
    cp $CONFIGSDIR/hosts $TARGET/etc/hosts
    # Copy new issue configuration file
    cp $CONFIGSDIR/issue $TARGET/etc/issue
    # Copy default hostname
    cp $CONFIGSDIR/hostname $TARGET/etc/hostname
    # Copy initramfs custom configuration and update initrd
    cp $CONFIGSDIR/initramfs/rootmount $TARGET/etc/initramfs-tools/scripts/init-bottom/rootmount
    cp $CONFIGSDIR/initramfs/modules   $TARGET/etc/initramfs-tools/modules
    chmod 0755 $TARGET/etc/initramfs-tools/scripts/init-bottom/rootmount
    mv $TARGET/etc/initramfs-tools/scripts/init-bottom/rootmount $TARGET/etc/initramfs-tools/scripts/init-bottom/__rootmount
    $CHROOT "$TARGET" /bin/bash -c "update-initramfs -u &>/dev/null"

    # Adding opendomo version
    echo "PRETTY_NAME=\"Opendomo OS version $OD_VERSION\""                                   > $TARGET/etc/os-release
    echo "NAME=\"Opendomo OS\""                                                             >> $TARGET/etc/os-release
    echo "VERSION_ID=$OD_VERSION"                                                           >> $TARGET/etc/os-release
    echo "VERSION=$OD_VERSION"                                                              >> $TARGET/etc/os-release
    echo "ID=opendomo"                                                                      >> $TARGET/etc/os-release
    echo "ANSI_COLOR=\"1;31\""                                                              >> $TARGET/etc/os-release
    echo "HOME_URL=http://es.opendomo.org/"                                                 >> $TARGET/etc/os-release
    echo "SUPPORT_URL=http://www.opendomo.com/wiki/index.php?title=P%C3%A1gina_Principal"   >> $TARGET/etc/os-release
    echo "BUG_REPORT_URL=https://github.com/opalenzuela/opendomo/issues"                    >> $TARGET/etc/os-release

    # Delete services (no used anymore)
#   $CHROOT "$TARGET" /bin/bash -c "insserv -rf mountall-bootclean.sh"
#   $CHROOT "$TARGET" /bin/bash -c "insserv -rf mountall.sh"
#   $CHROOT "$TARGET" /bin/bash -c "insserv -rf mtab.sh"
#   $CHROOT "$TARGET" /bin/bash -c "insserv -rf checkfs.sh"
#   $CHROOT "$TARGET" /bin/bash -c "insserv -rf checkroot-bootclean.sh"
#   $CHROOT "$TARGET" /bin/bash -c "insserv -rf checkroot.sh"
#   $CHROOT "$TARGET" /bin/bash -c "insserv -rf mountnfs-bootclean.sh"
#   $CHROOT "$TARGET" /bin/bash -c "insserv -rf mountnfs.sh"
#   $CHROOT "$TARGET" /bin/bash -c "insserv -rf hwclock.sh"

    # Clean packages
    $CHROOT "$TARGET" /bin/bash -c "apt-get clean"
}

configure_arm() {
    # Copy new modules file (RaspberryPi sound module added)
    cp $CONFIGSDIR/modules.arm $TARGET/etc/modules

    # Stoping unused services
#   $CHROOT "$TARGET" /bin/bash -c "insserv -rf plymouth-log"
#   $CHROOT "$TARGET" /bin/bash -c "insserv -rf plymouth"
    rm $TARGET/usr/bin/qemu-arm-static 2>/dev/null
}

configure_i386() {
    # Copy new modules file (Nothing added for now)
    cp $CONFIGSDIR/modules.i386 $TARGET/etc/modules
}

installpkg_i386() {
    # Install extra packages
    cp /etc/resolv.conf $TARGET/etc/
    echo "deb http://http.debian.net/debian stable main contrib non-free"			> $TARGET/etc/apt/sources.list
    $CHROOT "$TARGET" /bin/bash -c "apt-get update"
    $CHROOT "$TARGET" /bin/bash -c "LC_ALL=C LANGUAGE=C LANG=C DEBIAN_FRONTEND=noninteractive apt-get --no-install-recommends -yq install $ALLPACKAGES"
}

installpkg_arm() {
    # Install extra packages and firmware
    cp /etc/resolv.conf $TARGET/etc/
    echo "deb http://archive.raspbian.org/raspbian wheezy main firmware contrib non-free"	> $TARGET/etc/apt/sources.list
    $CHROOT "$TARGET" /bin/bash -c "apt-get update"
    $CHROOT "$TARGET" /bin/bash -c "LC_ALL=C LANGUAGE=C LANG=C DEBIAN_FRONTEND=noninteractive apt-get --no-install-recommends --force-yes -yq install $ALLPACKAGES"
    $CHROOT "$TARGET" /bin/bash -c "LC_ALL=C LANGUAGE=C LANG=C DEBIAN_FRONTEND=noninteractive apt-get --no-install-recommends --force-yes -yq install $ARMPACKAGES"
}

download_kernel() {
    # Install kernel and update initramfs
    wget -q --directory-prefix=$TARGET/tmp --no-check-certificate $KERNLIMGLURL
    wget -q --directory-prefix=$TARGET/tmp --no-check-certificate $INITRAMFSURL
    $CHROOT "$TARGET" /bin/bash -c "LC_ALL=C LANGUAGE=C LANG=C DEBIAN_FRONTEND=noninteractive cd /tmp && dpkg -i *.deb"
    rm $TARGET/tmp/* 2> /dev/null
}

fix_multistrap_configs() {
    # Correct bug in arm (create basic users and groups).
    cp $TARGET/usr/share/base-passwd/passwd.master $TARGET/etc/passwd
    cp $TARGET/usr/share/base-passwd/group.master  $TARGET/etc/group

    # Corrent bug in arm (dash broken), is necessary select "no" in question
    $CHROOT "$TARGET" /bin/bash -c "echo -e n | (LC_ALL=C LANGUAGE=C LANG=C dpkg-reconfigure --force dash)"
}


case $2 in
    build )
        echo; echo -e "[${INFO} INFO  ${NORL}] Building image (1/4) ..."; echo
        rm -r $TARGET 2> /dev/null

        if LC_ALL=C LANGUAGE=C LANG=C $MULTISTRAP -f $CONFIG; then
            # Selecting emulator for raspberrypi systems
            [ $DEVICE = arm ] && cp /usr/bin/qemu-arm-static $TARGET/usr/bin/qemu-arm-static 2>/dev/null

            # Mount kernel fs
            mkdir -p $TARGET/proc $TARGET/sys $TARGET/dev $TARGET/dev/pts
            mount -t proc /proc/ $TARGET/proc/
            mount --rbind /sys/ $TARGET/sys/
            mount --rbind /dev/ $TARGET/dev/
            mount --rbind /dev/pts $TARGET/dev/pts

            # Fix multistrap instalation
            echo -e "[${INFO} INFO  ${NORL}] Configuring packages (2/4) ..."
            fix_multistrap_configs &>/dev/null

            # Install packages
            echo -e "[${INFO} INFO  ${NORL}] Installing extra packages (3/4) ..."; echo
            installpkg_$DEVICE

            # Install kernel and new initramfs-tools
            echo; echo -e "[${INFO} INFO  ${NORL}] Downloading and installing kernel (4/4) ..."; echo
            download_kernel

            # Unmount kernel fs
            umount -fl $TARGET/dev/pts
            umount -fl $TARGET/dev/
            umount -fl $TARGET/sys/
            umount -fl $TARGET/proc/
        else
             echo; echo -e "[${ERRO} ERROR ${NORL}] Multistrap fail. Check configuration"; echo
        fi
    ;;
    make )
        # Selecting emulator for raspberrypi systems
        [ $DEVICE = arm ] && cp /usr/bin/qemu-arm-static $TARGET/usr/bin/qemu-arm-static 2>/dev/null

        # Configure image
        echo -e "[${INFO} INFO  ${NORL}] Configuring image (1/3) ..."
        configure_all
        configure_$DEVICE

        # Creating package
	echo -e "[${INFO} INFO  ${NORL}] Creating package (2/3) ..."
        tar cfp $TARGET.tar $TARGET
	echo -e "[${INFO} INFO  ${NORL}] Compressing package (3/3) ..."
        xz -z $TARGET.tar
        chown `users | cut -f1 -d" "` $TARGET.tar.xz
    ;;
esac
