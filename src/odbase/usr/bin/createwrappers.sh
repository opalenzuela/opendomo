#!/bin/sh
#desc:Create script wrappers
#package:odbase

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

cd /
for i in `find /usr/local/opendomo/services/ -type f`
do
    bn=`basename $i`
	# Only create wrapper if it does not exist
    if ! test -f /usr/local/opendomo/$i; then
		# Force execution privileges
		chmod +x $i
		# Create symbolic link
		ln -fs $i /usr/local/opendomo/ >/dev/null
		# Read possible group information
		GROUP=`grep '#group' $i | cut -f2 -d:`
		# If group information is found, apply
		test "$GROUP" = "users" && chmod g+rx $i 2>/dev/null
    fi
done
# Create CGI path links
ROOTPATH="/var/opendomo/cgiroot"
SCRIPTPATH="/usr/local/opendomo"
for section in config control map tools
do
    if test -d  "$SCRIPTPATH/services/$section"
    then
	cd $SCRIPTPATH/services/$section
	for i in *.sh
	do
	    mkdir -p $ROOTPATH/$section
	    ln -fs $SCRIPTPATH/$i $ROOTPATH/$section/$i
	    grep '#desc' $i | head -n1| cut -f2 -d: > $ROOTPATH/$section/$i.name
	done
    fi
done

# Find and delete removed wrappers
ODDIR="/usr/local/opendomo"
CGIDIR="/var/opendomo/cgiroot"
find -L $ODDIR/ -type l -delete
find -L $CGIDIR/ -type l -delete

# Also, force execution for daemons and eventhandlers
chmod +x /usr/local/opendomo/daemons/*.sh 	2>/dev/null
chmod +x /usr/local/opendomo/eventhandlers/*.sh 2>/dev/null
chmod +x /var/www/cgi-bin/*.* 2>/dev/null

# Cleaning possible Windows encoding EOL
DIRS="$ODDIR/bin/ $ODDIR/daemons/ $ODDIR/eventhandlers/ $ODDIR/services/syscript/ \
      $ODDIR/services/config/ $ODDIR/services/control/ $ODDIR/services/tools/"

for dir in $DIRS; do
	test -z `ls $dir/* | head -c1` || sed -e 's/\r//g' -i $dir/*.sh
done
