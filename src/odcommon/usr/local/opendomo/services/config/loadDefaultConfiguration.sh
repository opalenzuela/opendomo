#!/bin/sh
#desc:Load default configuration
#package:odcommon
#type:local

#TODO: Need rebuild
exit 0

if test -z "$1"; then
	# No params. Showing menu and warning
	echo "#WARN: Default configuration will be loaded"
	echo "#> Restoring configuration"
	echo "form:`basename $0`"
	echo "# Do you want to continue?"
	echo "	confirm	confirm	hidden	yes"
	echo "action:"
	echo "	`basename $0`	Yes"
	echo
else
	cd /mnt/odconf/
	if test -f pdefault/sysconf.tar.gz; then
		if test -w sysconf/sysconf.tar.gz; then
			echo "# Restoring configuration"
			echo "#INF: Please, reboot your system"
			echo "action:"
			echo
			echo
			rm sysconf/sysconf.tar.gz 2>/dev/null
			rm pkgcache/* 2>/dev/null
			cp pdefault/sysconf.tar.gz sysconf/
			cp pdefault/*.gz pkgcache/
			rm pkgcache/sysconf*
			echo
		else
			echo "#ERROR: Permission denied"
			echo
			exit 1
		fi
	else
		echo "#ERROR: Default configuration not found"
		echo
		exit 2
	fi
fi
