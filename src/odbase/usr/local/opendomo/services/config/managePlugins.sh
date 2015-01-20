#!/bin/sh
#desc:Manage plugins
#type:local
#package:odbase

# Copyright(c) 2015 OpenDomo Services SL. Licensed under GPL v3 or later

ODAPTPID=/var/opendomo/run/opendomo-apt.pid
STATFILE=/var/opendomo/tmp/plugin_status.tmp
DOWNDIR=/var/opendomo/apt/plugins
INSTDIR=/var/opendomo/plugins

IFS=$'\x0A'$'\x0D'
source /etc/os-release

PLUGINREPOSITORY="http://cloud.opendomo.com/repo/$VERSION"
REPOTEMP=/var/opendomo/apt/repository.tmp
REPOFILE=/var/opendomo/apt/repository.lst

# Check plugin status
echo "INPROGRES=\"`ls -1 $DOWNDIR |  tr '$\n' " "`\""                       > $STATFILE
echo "INSTALLED=\"`ls -1 $INSTDIR | cut -f1 -d. | uniq | tr '$\n' " "`\""  >> $STATFILE
source $STATFILE

# opendomo-apt is in "Spaceless" mode, plugins can be installed
grep Spaceless $ODAPTPID &>/dev/null && echo "#WARN No free space available, you need save configuration and reboot to install plugins"

# Repository don't exists or empty, force download
if test -z `cat $REPOFILE 2>/dev/null | head -c2`; then
	wget $PLUGINREPOSITORY/ -O $REPOTEMP --no-check-certificate --max-redirect=0 &>/dev/null && grep -v "#" $REPOTEMP | grep "-" > $REPOFILE
fi

if test -z "$1"; then
	echo "#> Manage plugins"
	echo  "list:managePlugins.sh	iconlist selectable"
	for plugin in `cat $REPOFILE |  cut -f1 -d- | uniq`; do
		# Extract plugin information
		ID=`grep $plugin $REPOFILE | cut -f1 -d'-' | head -n1`
		DESC=`grep $plugin $REPOFILE | cut -f3 -d';' | head -n1`
		test -z "$DESC" && DESC="$ID"

		if ! test -f /var/www/data/$ID.png ; then
			wget -q http://es.opendomo.org/files/$ID.png -O /var/www/data/$ID.png
		fi
		
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
	echo "	installPlugin.sh	Install"
	grep Spaceless $ODAPTPID &>/dev/null && echo "	saveConfigReboot.sh	Save and reboot"
	if test -x /usr/local/opendomo/installPluginFromGithub.sh
	then
		echo "	installPluginFromGithub.sh	Install from Github"
	fi
	if test -x /usr/local/opendomo/updateSystem.sh
	then
		echo "	updateSystem.sh	Update system"
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
	if test -f /var/opendomo/plugins/$1.info; then
		source /var/opendomo/plugins/$1.info
	fi
	
	if test -z "$REPOSITORY" || test -z "$AUTHORID"
	then
		# If website is not specified, we use the community one.
		WEB="http://es.opendomo.org/"
	else
		WEB="https://raw.githubusercontent.com/$AUTHORID/$REPOSITORY/master/"
	fi
	
	# Check status
	if   [ `echo $INSTALLED | grep -c1 $1` == "1" ]; then
		STATUS="installed"
	elif [ `echo $INPROGRES | grep -c1 $1` == "1" ]; then
		STATUS="inprogress"
	else
		STATUS="new"
	fi	
	CODE=`echo $1 | cut -f1 -d.`
	echo "#> Plugin"
	echo "form:managePlugins.sh	"
	#echo "	icon	Icon	image	$WEB/$1.png"
	echo "	code	Code	readonly	$CODE"
	echo "actions:"
	echo "	goback	Back"
	test -f "$INSTDIR/$1.files" && echo "	removePlugin.sh	Uninstall"
	test -f "$DOWNDIR/$FILE" || test -f "$INSTDIR/$1.files" || echo "	installPlugin.sh	Install"
	echo	
	echo "#> Plugin details"
	echo "form:seePluginDetails.sh"
	echo "	desc	Description	readonly	$DESC"
	echo "	deps	Dependences	readonly	$DEPS"
	echo "	webp	Web page	readonly	$WEB"
	echo "	stat 	Status	readonly	$STATUS"
	echo "actions:"	
	echo
	if wget -q $WEB/CHANGELOG -O /var/opendomo/tmp/$1.changelog
	then
		echo "#> Last changes"
		echo "form:seePluginDetails.sh	foldable"	
		head -n10 /var/opendomo/tmp/$1.changelog | sed 's/^/# /'
		echo "actions:"		
	fi
fi
echo
