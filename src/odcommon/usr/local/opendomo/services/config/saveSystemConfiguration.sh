#!/bin/sh
#desc:Save configuration
#package:odcommon
#type:multiple

CFGFILE="/mnt/odconf/sysconf/sysconf.tar"
LOG="/var/log/savecfg.log"
ERR="/var/log/savecfg.err"
ENC="/tmp/cfgbackup.enc"

save_data(){
	if test -d /mnt/odconf/ ; then
		echo "#> Saving data"
		echo
		echo
		echo
		echo "list:`basename $0`"
		mkdir -p /mnt/odconf/sysconf  || (echo "#ERR Invalid saving path" && exit 1)

		#Unprotect config file
		chmod 0600 $CFGFILE 2>/dev/null
		if touch $CFGFILE 2>/dev/null
		then
			cd /
			FILES="etc/ssh/ssh_host* etc/opendomo/ etc/passwd etc/shadow"
			if tar cvf $CFGFILE $FILES >> $LOG 2> $ERR
			then
				if test -f $CFGFILE.gz; then
					rm $CFGFILE.gz
				fi
				gzip $CFGFILE
				sync
			else
				echo "#ERR Error saving configuration:"
				cat /var/log/savecfg.err | sed 's/^/# /'
				echo
				exit 3
			fi
		else
			echo "#ERR Configuration path is read-only"
			echo
			exit 2
		fi

		#Protect file
	  	chmod 0400 $CFGFILE 2>/dev/null
	else
		echo "# OpenDomo cannot find a proper unit to save configuration"
		echo "# Find how to solve this problem clicking the button 'More information'"
		exit 1
	fi
}

case "$1" in
	admin)
		# Saving data
		save_data
		# Encrypt
		openssl enc -aes-256-cbc -salt -in $CFGFILE.gz -out $ENC -pass pass:$2

		echo "# Configuration saved"
		echo "# To perform a system upgrade, is necessary to upload the configuration to the Cloud"
		echo "action:"
		echo "	uploadSystemConfiguration.sh	Upload configuration to the Cloud"
		echo
		;;

	upload)
		echo "#INFO Uploading configuration file"
		echo
		curl -F file=$ENC http://cloud.opendomo.com/upgrade/
		echo
		;;

	*)
		# None
			echo "#> Save configuration"
			echo "#INFO: Validation required"
			echo "form:`basename $0`"
			echo "	USER	Username	text"
			echo "	PASS	Password	password"
			echo
			exit 0
		;;
esac

