#! /bin/sh
#desc: Opendomo configuration manager

# Variables
DIRCONF="/etc/opendomo"
DEFCONF="/mnt/odconf/sysconf/defconf.tar"
CSTCONF="/mnt/odconf/sysconf/cstconf.tar"
LOGFILE="/var/opendomo/log/loadcfg.log"
DEFAULTSDIR="/usr/local/opendomo/defaults"

help () {
  echo "USAGE:"
  echo "   manage_conf.sh load custom / default  - Load conf"
  echo "   manage_conf.sh save                   - Save custom conf "
  echo "   manage_conf.sh restore                - Restore default conf"
  echo "   manage_conf.sh copy                   - Copy plugin default config to /etc"
  echo
}

load_default_config () {
  if test -f $DEFCONF.gz; then
	echo "INFO: Load default configuration file ..."
	tar xfzp $DEFCONF.gz -C $DIRCONF 2>>$LOGFILE
  else
	echo "ERROR: Default config don't exist"
  fi
}

load_custom_config () {
  if test -f $CSTCONF.gz; then
	echo "INFO: Load custom configuration file ..."
	tar xfzp $CSTCONF.gz -C $DIRCONF 2>>$LOGFILE
  else
	echo "ERROR: Custom config don't exist"
	exit 1
  fi
}

save_custom_config () {
  # Delete previous config file
  rm $CSTCONF.gz 2>>$LOGFILE

  # Save config
  cd $DIRCONF
  if tar cfp $CSTCONF * --owner 1000 --group 1000 2>>$LOGFILE ; then
	gzip $CSTCONF >/dev/null 2>/dev/null
	echo "INFO: Configuration saved"
  else
	echo "ERROR: Configuration can't be saved"
  fi
}

case $1 in
  load )
  if test -z $2; then
	echo "ERROR: You need select custom or default file"
	exit 1
  else
	case $2 in
		custom )
		load_custom_config
		;;
		default )
		load_default_config
		;;
		* )
			echo "ERROR: You need select custom or default file"
			exit 1
		;;
		esac
	fi
  ;;
  save )
	save_custom_config
  ;;
  restore )
	echo "INFO: Restore default config ..."

	rm $CSTCONF.gz 2>>$LOGFILE
	rm -r $CONFDIR/* 2>$LOGFILE
	load_default_config
  ;;
  copy )
	echo "INFO: Copy default configurations ..."
	cd $DEFAULTSDIR
	for file in `find ./ | sed 's/^..//'`; do

		# Only copy if no exist
		if ! test -f $CONFDIR/$file; then
			cp -pr $DEFAULTSDIR/$file $CONFDIR/$file 2>>$LOGFILE
		fi
	done

	#TODO Create system links
 ;;
  * )
	help
  ;;
esac
