#!/bin/sh
#desc:Network interface configuration
#type:local
#package:odcommon

CFGFILE="/etc/opendomo/system/interfaces"

if ! test -z "$1"; then
	MODE="$1"
	IPADDRESS="$2"
	NETMASK="$3"
	DEFAULTGW="$4"
	DNSSERVER="$5"
	case "$1" in
		static)

			if test -z "$IPADDRESS" || test -z "$DEFAULTGW"; then
				echo "#ERR Missing parameters"
			else
				echo "auto lo" > $CFGFILE
				echo "iface lo inet loopback" >> $CFGFILE
				echo "" >> $CFGFILE
				echo "auto eth0" >> $CFGFILE
				echo "iface eth0 inet static" >> $CFGFILE
				echo "address $IPADDRESS" >> $CFGFILE
				echo "netmask $NETMASK" >> $CFGFILE
				echo "gateway $DEFAULTGW" >> $CFGFILE
				echo "dns-nameservers $DNSSERVER" >> $CFGFILE

				echo "#INFO Configuration saved"
				echo "#INFO Please, save configuration and restart"
			fi
		;;
		dhcp)
			echo "auto lo" > $CFGFILE
			echo "iface lo inet loopback" >> $CFGFILE
			echo "" >> $CFGFILE
			echo "auto eth0" >> $CFGFILE
			echo "iface eth0 inet dhcp" >> $CFGFILE
			echo "#INFO Configuration saved"
			echo "#INFO Please, save configuration and restart"
		;;
		*)
			echo "#ERROR Invalid parameter"
			echo
		;;
	esac
else
	# First load of the page
	if grep -q dhcp /etc/network/interfaces
	then
		MODE=dhcp
	else
		MODE=static
		IPADDRESS=`grep address $CFGFILE | cut -f2 -d' '`
		NETMASK=`grep netmask $CFGFILE | cut -f2 -d' '`
		DEFAULTGW=`grep gateway $CFGFILE | cut -f2 -d' '`
		DNSSERVER=`grep -m1 dns-nameservers $CFGFILE | cut -f2 -d' '`
	fi
fi

echo "#> Network interface configuration"
echo "form:`basename $0`"
echo "	mode	Mode	list[static:static,dhcp:DHCP client]	$MODE"
echo "	ip	IP	text	$IPADDRESS"
echo "	mask	Netmask	text	$NETMASK"
echo "	gw	Gateway	text	$DEFAULTGW"
echo "	dns	DNS server	text	$DNSSERVER"
echo
