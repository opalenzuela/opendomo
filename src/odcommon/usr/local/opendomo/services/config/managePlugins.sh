#!/bin/sh
#desc:Manage plugins
#type:local
#package:odcommon

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

ODAPTPID=/var/opendomo/run/opendomo-apt.pid
STATFILE=/var/opendomo/tmp/plugin_status.tmp
DOWNDIR=/var/opendomo/apt/plugins
INSTDIR=/var/opendomo/plugins

IFS=$'\x0A'$'\x0D'
source /etc/os-release

REPOSITORY="http://cloud.opendomo.com/repo/$VERSION"
REPOTEMP=/var/opendomo/apt/repository.tmp
REPOFILE=/var/opendomo/apt/repository.lst

# Check plugin status
echo "INPROGRES=\"`ls -1 $DOWNDIR |  tr '$\n' " "`\""                       > $STATFILE
echo "INSTALLED=\"`ls -1 $INSTDIR | cut -f1 -d. | uniq | tr '$\n' " "`\""  >> $STATFILE
source $STATFILE

# opendomo-apt is in "Spaceless" mode, plugins can be installed
grep Spaceless $ODAPTPID &>/dev/null && echo "#WARN No free space available, you need save configuration and reboot to install plugins"

# Repository don't exists or emply, force download
if test -z `cat $REPOFILE 2>/dev/null | head -c2`; then
	wget $REPOSITORY/ -O $REPOTEMP --no-check-certificate --max-redirect=0 &>/dev/null && grep -v "#" $REPOTEMP | grep "-" > $REPOFILE
fi

if test -z "$1"; then
	echo "#> Manage plugins"
	echo  "list:managePlugins.sh	iconlist"
	for plugin in `cat $REPOFILE |  cut -f1 -d- | uniq`; do
		# Extract plugin information
		ID=`grep $plugin $REPOFILE | cut -f1 -d'-' | head -n1`
		DESC=`grep $plugin $REPOFILE | cut -f3 -d';' | head -n1`
		test -z "$DESC" && DESC="$ID"

		# Check status
		if   [ `echo $INSTALLED | grep -c1 $ID` == "1" ]; then
			STATUS="installed"
		elif [ `echo $INPROGRES | grep -c1 $ID` == "1" ]; then
			STATUS="inprogress"
		else
			STATUS="new"
		fi

		echo "	-$ID	$DESC	image plugin $STATUS	$STATUS"
	done

	echo "actions:"
	echo "	managePlugins.sh	Details"
	grep Spaceless $ODAPTPID &>/dev/null && echo "	saveConfigReboot.sh	Save and reboot"
	if test -x /usr/local/opendomo/installPluginFromGithub.sh
	then
		echo "	installPluginFromGithub.sh	Install from Github"
	fi
else
	# Parameter was passed (requesting plugin's details)
	if test "$1" == "kernel"
	then
		FILE="linux"
		URL="http://linux.org"
		DESC="Linux kernel"
		DEPS="none"
		WEB="http://linux.org"
	else
		
		FILE=`grep ^$1 $REPOFILE | sort -r  | head -n1 | cut -f1 -d';' `
		URL=`grep ^$1 $REPOFILE  | sort -r  | head -n1 | cut -f2 -d';' `
		DESC=`grep ^$1 $REPOFILE | sort -r  | head -n1 | cut -f3 -d';' `
		DEPS=`grep ^$1 $REPOFILE | sort -r  | head -n1 | cut -f4 -d';' `
		WEB=`grep ^$1 $REPOFILE  | head -n1 | cut -f6 -d';' `
	fi
	if test -z "$DEPS"
	then
		DEPS="none"
	fi
	if test -z "$WEB"
	then
		# If website is not specified, we use the community one.
		WEB="http://es.opendomo.org/"
	fi
	
	# Check status
	if   [ `echo $INSTALLED | grep -c1 $1` == "1" ]; then
		STATUS="installed"
	elif [ `echo $INPROGRES | grep -c1 $1` == "1" ]; then
		STATUS="inprogress"
	else
		STATUS="new"
	fi	
	
	echo "#> Plugin details"
	echo "form:managePlugins.sh	"
	#echo "	icon	Icon	image	$WEB/$1.png"
	echo "	code	Code	readonly	$1"
	echo "	desc	Description	readonly	$DESC"
	echo "	deps	Dependences	readonly	$DEPS"
	echo "	webp	Web page	readonly	$WEB"
	echo "	stat 	Status	readonly	$STATUS"
	
	echo "actions:"
	echo "	goback	Back"
	test -f "$INSTDIR/$1.files" && echo "	removePlugin.sh	Uninstall"
	test -f "$DOWNDIR/$FILE" || test -f "$INSTDIR/$1.files" || echo "	installPlugin.sh	Install"
fi
echo
