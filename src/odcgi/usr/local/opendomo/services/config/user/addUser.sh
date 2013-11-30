#!/bin/sh
#desc:Add user
#type:multiple
#package:odcgi

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

mkdir -p /etc/opendomo/udata

if test -z "$4"; 
then
	echo "#> User data"
	echo "form:`basename $0`"
	echo "	user	Name	text	$1"
	echo "	fullname	Full name	text	$2"	
	echo "	newpasss	Password	password"
	echo "	retype	Repeat password	password"
	echo "	email	E-mail	text"
	echo "	adminp	Admin password	password"
else
	if test "$3" = "$4"; then
		if /usr/bin/usermm $6 add $1 $3; then
			mkdir -p /etc/opendomo/udata/$1/
			if ! test -z "$2"; then
				echo "$2" | sed 's/+/ /g' > /etc/opendomo/udata/$1/name
			fi
			if ! test -z "$5"; then
				echo "$5" > /etc/opendomo/udata/$1/email
			else
				rm /etc/opendomo/udata/$1/email
			fi
			echo "#INFO User [$1] created"
		else
			echo "#ERR Permission denied"
			exit 2
		fi
	else
		echo "#ERR Passwords doesn't match"
		exit 1
	fi
fi
echo 

