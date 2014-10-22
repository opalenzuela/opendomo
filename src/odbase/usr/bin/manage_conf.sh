#! /bin/sh
#desc: Opendomo configuration manager

# Variables
DIRCONF="/etc/opendomo"
LOGFILE="/var/opendomo/log/loadcfg.log"
DEFAULTSDIR="/usr/local/opendomo/defaults"
DEFSYSDIR="/usr/local/opendomo/defaults/system"
SYSTEMDIR="/etc/opendomo/system"
BLACKLIST="group group- gshadow gshadow- shadow shadow- passwd passwd- sudoers sudoers.d VERSION debian_version os-release os-release.orig mtab"

help () {
  echo "USAGE:"
  echo "   manage_conf.sh restore                - Restore system conf files"
  echo "   manage_conf.sh copy                   - Copy plugin default config to /etc"
  echo "   manage_conf.sh blacklist              - Check system blacklist files"
  echo
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
  restore )
	echo "INFO: Restoring system configs ..."
	restore_system_conf
  ;;
  copy )
	echo "INFO: Copy default configurations ..."
	copy_default_conf

	echo "INFO Creating system config links ..."
	create_system_conf
  ;;
  blacklist )
	echo "INFO Checking blacklist files ..."
	check_blacklist
  ;;
  * )
	help
  ;;
esac
