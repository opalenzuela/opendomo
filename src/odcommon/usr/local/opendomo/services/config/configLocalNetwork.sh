#!/bin/sh
#desc:Network interface configuration
#type:local
#package:odcommon

#TODO: Needs a rebuild
exit 0

if ! test -z "$1"; then
	MODE="$1"
	IPADDRESS="$2"
	DEFAULTGW="$3"
	NTPSERVER="$4"
	DNSSERVER="$5"
	case "$1" in
		static)
			if test -z "$IPADDRESS" || test -z "$DEFAULTGW"; then
				echo "#ERR Missing parameters"	
			else
				rm -f /etc/opendomo/dhcpd.conf
				echo "IPADDRESS=$IPADDRESS" > /etc/opendomo/network.conf
				echo "DEFAULTGW=$DEFAULTGW" >> /etc/opendomo/network.conf
				echo "NTPSERVER=$NTPSERVER" >> /etc/opendomo/network.conf
				echo "DNSSERVER=$DNSSERVER" >> /etc/opendomo/network.conf
				chown admin /etc/opendomo/network.conf
				echo "#INF: Configuration saved"
				echo "#INF: Please, save configuration and restart"
			fi
		;;
		dhcp)
			rm -f /etc/opendomo/dhcpd.conf
			rm -f /etc/opendomo/network.conf
			echo "#INF: Configuration saved"
			echo "#INF: Please, save configuration and restart"
		;;
		server)
			rm -f /etc/opendomo/network.conf
			if ! test -f /etc/opendomo/dhcpd.conf; then
				cp /etc/udhcpd.conf /etc/opendomo/dhcpd.conf
			fi
			echo "#INF: Configuration saved"
			echo "#INF: Please, save configuration and restart"
		;;
		*)
			if test -f /etc/opendomo/network.conf; then
				. /etc/opendomo/network.conf
			fi
		;;
	esac
else
	# First load of the page
	if test -f /etc/opendomo/network.conf; then
		MODE=static
		. /etc/opendomo/network.conf
	else	
		if test -f /etc/opendomo/dhcpd.conf; then
			MODE=server
		else
			MODE=dhcp
		fi
	fi
fi

echo "#> Network interface configuration"
echo "form:`basename $0`"
#echo "	mode	Mode	list[static:static,server:DHCP server,dhcp:DHCP client]	$MODE"
echo "	mode	Mode	list[static:static,dhcp:DHCP client]	$MODE"
echo "	ip	IP	text	$IPADDRESS"
echo "	gw	Gateway	text	$DEFAULTGW"
echo "	ntp	Time server	text	$NTPSERVER"
echo "	dns	DNS server	text	$DNSSERVER"
echo
