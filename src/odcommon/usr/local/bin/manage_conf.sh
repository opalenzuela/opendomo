#! /bin/sh
#desc: Opendomo configuration manager

# Variables
DIRCONF="/etc/opendomo"
DEFCONF="/mnt/odconf/sysconf/defconf.tar"
CSTCONF="/mnt/odconf/sysconf/cstconf.tar"
LOGFILE="/var/opendomo/log/loadcfg.log"
DEFAULTSDIR="/usr/local/opendomo/defaults"
DEFSYSDIR="/usr/local/opendomo/defaults/system"
SYSTEMDIR="/etc/opendomo/system"
BLACKLIST="group group- gshadow gshadow- shadow shadow- passwd passwd- sudoers sudoers.d VERSION debian_version os-release os-release.orig mtab"

help () {
  echo "USAGE:"
  echo "   manage_conf.sh load custom / default  - Load conf"
  echo "   manage_conf.sh save                   - Save custom conf "
  echo "   manage_conf.sh restore                - Restore default conf"
  echo "   manage_conf.sh restore system         - Restore system conf files"
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

copy_default_conf () {
  cd $DEFAULTSDIR

  # Check empty dir
  if test -z `find -maxdepth 0 -empty`; then
	# Find default files
	for file in `find ./ | sed 's/^..//'`; do

		# Create dirs and only copy files if don't exist
		if test -d $file; then
			mkdir -p $DIRCONF/$file
			chown admin:admin $DIRCONF/$file
		else
			if ! test -f $DIRCONF/$file; then
				cp -p $DEFAULTSDIR/$file $DIRCONF/$file 2>>$LOGFILE
				chown admin:admin $DIRCONF/$file
			fi
		fi
  	done
  fi
}

check_blacklist () {
  for blackconf in $BLACKLIST; do
        if test -f "$DEFSYSDIR/$blackconf"; then
		echo "#ERR Configuration file '$blackconf' can be used by plugins, cleaning ..."
		rm $blackconf 2>/dev/null
	fi
  done
}

create_system_conf () {
  cd $DEFSYSDIR

  # Check empty dir
  if test -z `find -maxdepth 0 -empty`; then
	# Find system file
	for config in *; do
		SYSTEMCONF=`find /etc/ -name $config | grep -m1 -v opendomo`
		SYSDIRCONF=`dirname $SYSTEMCONF`

		# If file is a link do nothing, if not create link
		if ! test -h $SYSTEMCONF; then
			mv $SYSTEMCONF $SYSTEMDIR/$config.origin
			ln -s $SYSTEMDIR/$config $SYSDIRCONF/$config
		fi
	done
  fi
}

restore_system_conf () {
  cd $SYSTEMDIR

  # Check empty dir
  if test -z `find -maxdepth 0 -empty`; then
	# Restore original configs
	for config in `ls -1 | grep -v origin`; do
		SYSTEMCONF=`find /etc/ -name $config | grep -m1 -v opendomo`
		SYSDIRCONF=`dirname $SYSTEMCONF`
		if ! test -e $DEFSYSDIR/$config; then
			if test -h $SYSDIRCONF/$config; then

				# Delete custom files and restore original
				rm $SYSDIRCONF/$config
				rm $config
				mv $config.origin $SYSDIRCONF/$config
			fi
		fi
	done
  fi
}

# Make folders
mkdir -p $DEFSYSDIR $SYSTEMDIR $DEFAULTSDIR

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
	if [ "$2" = "system" ]; then
		echo "INFO: Restoring system configs ..."
		restore_system_conf
	else
		echo "INFO: Restore default config ..."
		rm $CSTCONF.gz 2>>$LOGFILE

		# Exclude system configuration folders
		cd $DIRCONF
		for file in `ls -1 | grep -v system`; do
			rm -r $file 2>>$LOGFILE
		done
		load_default_config
	fi
  ;;
  copy )
	echo "INFO: Copy default configurations ..."
	copy_default_conf

	echo "INFO Creating system config links ..."
	create_system_conf
  ;;
  blacklist )
	check_blacklist
  ;;
  * )
	help
  ;;
esac
