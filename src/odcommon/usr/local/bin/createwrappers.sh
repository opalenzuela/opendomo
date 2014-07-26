#!/bin/sh
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

# Also, force execution for daemons and eventhandlers
chmod +x /usr/local/opendomo/daemons/*.sh 	2>/dev/null
chmod +x /usr/local/opendomo/eventhandlers/*.sh 2>/dev/null
# Cleaning possible Windows encoding EOL
sed -e 's/\r//g' -i /usr/local/opendomo/*.sh	           2>/dev/null
sed -e 's/\r//g' -i /usr/local/opendomo/bin/*.sh           2>/dev/null
sed -e 's/\r//g' -i /usr/local/opendomo/daemons/*.sh       2>/dev/null
sed -e 's/\r//g' -i /usr/local/opendomo/eventhandlers/*.sh 2>/dev/null
