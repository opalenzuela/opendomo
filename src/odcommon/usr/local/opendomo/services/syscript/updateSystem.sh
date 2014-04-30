$ nano saveSystemConfig.sh 
$ nano updateSystem.sh 
$ cat updateSystem.sh 
#!/bin/sh
#desc: Update debian and opendomo packages

# Checking arch
if [ `arch` != "armv6l" ]; then
	ARCH="i386"
else
	ARCH="armhf"
fi

VERSION="2.0.1"
ODCOMPKG="odcommon_"$VERSION"_"$ARCH".deb"
ODCGIPKG="odcgi_"$VERSION"_"$ARCH".deb"
ODHALPKG="odhal_"$VERSION"_"$ARCH".deb"
PKGURL="http://es.opendomo.org/files"

LOGFILE="/var/opendomo/log/updateSystem.log"
APTFILE="/var/opendomo/apt-lastupdate.info"
CURWEEK=`date +%W`
PIDFILE="/var/opendomo/run/updateSystem.pid"

# Check process
if test -f $PIDFILE; then
    echo "#ERRO System update is already running"
fi
touch $PIDFILE
echo "#LOADING Updating opendomo packages, please wait ..."
echo

#Only update apt sources once a week
if ! test -f $APTFILE || [ $CURWEEK != `cat $APTFILE` ]
then
    echo "#INFO Updating sources ..." &>>$LOGFILE
    sudo apt-get update &>>$LOGFILE
    date +%W >$APTFILE
fi

LC_ALL=C LANGUAGE=C LANG=C DEBIAN_FRONTEND=noninteractive sudo apt-get --force-yes -yq upgrade &>>$LOGFILE
if wget $PKGURL/$ODCOMPKG $PKGURL/$ODCGIPKG $PKGURL/$ODHALPKG &>>$LOGFILE; then
    if sudo dpkg -i $ODCOMPKG $ODCGIPKG $ODHALPKG &>>$LOGFILE; then
        rm $ODCOMPKG $ODCGIPKG $ODHALPKG
    else
        # Fix dpkg database installing missing deps
        LC_ALL=C LANGUAGE=C LANG=C DEBIAN_FRONTEND=noninteractive sudo apt-get --force-yes -fyq install &>>$LOGFILE
        rm $ODCOMPKG $ODCGIPKG $ODHALPKG
    fi
fi

# Return to manage plugins
echo "#INFO System updated"
echo
/usr/local/opendomo/managePlugins.sh
rm $PIDFILE
