#!/bin/sh
#desc:Edit user
#type:multiple
#package:odcgi

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

#FIXME Provisional script. This will be replaced by the ./user/ toolkit in beta

PROGNAME=`basename $0`
#USER=admin

if ! test -d /etc/opendomo/udata/ ; then
	mkdir -p /etc/opendomo/udata/admin
	mkdir -p /etc/opendomo/udata/user
	echo "Administrator" > /etc/opendomo/udata/admin/name
	echo "Standard User" > /etc/opendomo/udata/user/name
fi


if test -z "$2"; then
	if test -f /etc/opendomo/udata/$USER/email; then
		email=`cat /etc/opendomo/udata/$USER/email`
	fi
	if test -f /etc/opendomo/udata/$USER/name; then
		name=`cat /etc/opendomo/udata/$USER/name`
	else
		name="$1"
	fi 
	echo "#> Change user data"
	echo "form:$PROGNAME"
	echo "	contact	Contact information	separator"
	echo "	user	Username	readonly	$USER"
	echo "	fullname	Full name	text:[a-zA-Z0-9 \.]+	$name"
	echo "	email	E-mail	text:[a-z0-9_\.]+@[a-z0-9\.]+	$email"
	echo "	cred	Credentials	separator"
	echo "	oldpass	Old password	password"
	echo "	newpass	New password	password"
	echo "	retype	Repeat password	password"

else
	# Modify User name ONLY IF SPECIFIED
	if ! test -z "$2"; then
		echo "$2" | sed 's/+/ /g' > /etc/opendomo/udata/$USER/name
	fi
	
	# Modify email ONLY IF SPECIFIED
	if ! test -z "$3"; then
		echo "$3" > /etc/opendomo/udata/$USER/email
	else
		rm /etc/opendomo/udata/$USER/email
	fi

	# Modify passwords ONLY IF SPECIFIED
	if ! test -z "$4"; then
		if test "$5" = "$6"; then
			if ! /usr/bin/usermm $4 set $1 $5; then
				echo "#ERR Permission denied"
				exit 2
			fi
		else
			echo "#ERR Passwords do not match"
			exit 1
		fi
	fi
	echo "#INFO Data modified for [$1]"
	
fi
echo 

