#!/bin/sh
#desc:Edit user
#type:multiple
#package:odcgi

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

PROGNAME=`basename $0`
USER=$1

if ! test -d /etc/opendomo/udata/ ; then
	mkdir -p /etc/opendomo/udata/admin
	mkdir -p /etc/opendomo/udata/user
	echo "Administrator" > /etc/opendomo/udata/admin/name
	echo "Standard User" > /etc/opendomo/udata/user/name
fi

cd /home
for u in *; do
	if test "$u" != "root"; then
		if test -z "$USERLIST"; then
			USERLIST="$u"
		else
			USERLIST="$USERLIST,$u"
		fi
	fi
done

if test -z "$1"
then
	echo "#> Available users"
	echo "list:$PROGNAME	iconlist"
	for u in *; do
		if test "$u" != "root"; then
			if test -f /etc/opendomo/udata/$u/name; then
				name=`cat /etc/opendomo/udata/$u/name`
			else
				name="$u"
			fi
			echo "	-$u	$name	user"
		fi
	done
	echo
else
	if test -z "$2"; then
		if test -f /etc/opendomo/udata/$1/email; then
			email=`cat /etc/opendomo/udata/$1/email`
		fi
		if test -f /etc/opendomo/udata/$1/name; then
			name=`cat /etc/opendomo/udata/$1/name`
		else
			name="$1"
		fi 
		echo "form:$PROGNAME"                         
		echo "	user	Username	readonly	$1"
		echo "	oldpass	Old password	password"
		echo "	newpass	New password	password"
		echo "	retype	Repeat password	password"
		echo "	fullname	Full name	text	$name"
		echo "	email	E-mail	text	$email"
	
	else
		if test "$3" = "$4"; then
			if /usr/bin/usermm $2 set $1 $3; then
				if ! test -z "$5"; then
					echo "$5" | sed 's/+/ /g' > /etc/opendomo/udata/$1/name
				fi
				if ! test -z "$6"; then
					echo "$6" > /etc/opendomo/udata/$1/email
				else
					rm /etc/opendomo/udata/$1/email
				fi
				echo "#INFO Password modified for [$1]"
			else
				echo "#ERR Permission denied"
				exit 2
			fi
		else
			echo "#ERR Passwords doesn't match"
			exit 1
		fi
	fi
fi
echo 

