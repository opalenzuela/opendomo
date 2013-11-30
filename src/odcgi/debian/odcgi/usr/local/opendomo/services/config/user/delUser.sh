#!/bin/sh
#desc:Delete user
#type:multiple
#package:odcgi

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

PROGNAME=`basename $0`
USER=$1

cd /home
for u in *; do
	if test "$u" != "root" && test "$u" != "admin"; then
		if test -z "$USERLIST"; then
			USERLIST="$u"
		else
			USERLIST="$USERLIST,$u"
		fi
	fi
done


if test -z "$2";
then        
	echo "form:$PROGNAME"                         
	echo "	user	Username	list[$USERLIST]	$USER"
	echo "	adminp	Admin password 	password"	
else                   
	if /usr/bin/usermm $2 rm $1; then
		echo "#INFO User [$USER] deleted"
	else
		echo "#ERR Permission denied"
		exit 2
	fi
fi	
echo

