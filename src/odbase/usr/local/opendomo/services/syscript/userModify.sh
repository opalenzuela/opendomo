#!/bin/sh
#desc:Manage users
#type:multiple
#package:odbase

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

USER="$1"
FULLNAME="$2"
EMAIL="$3"
PASSWD="$4"
GROUPUID="100"
USERGROUPS="disk,audio,video"

# Without params, display "Add user" interface (only name and mail required)
if test -z $1; then
    echo "#> Add user"
    echo "form:`basename $0`"
    #echo "	user	User	text	$USER"
    echo "	fullname	Full name	text $FULLNAME"
    echo "	email	Email	text	$EMAIL"
    #echo "	passw	Password	password	$PASSWD"
    echo "actions:"
    echo "	goback	Back"
    echo "	userModify.sh	Add"
    echo
	
# Only 2 parameters: creating new user with default password
elif ! test -z $1 && test -z $3; then	
	FULLNAME="$1"
	EMAIL="$2"
	USERNAME=`echo $EMAIL | cut -f1 -d@`
	PASSWD="opendomo"
	
    sudo usermod -c "$FULLNAME <$EMAIL>" $USERNAME  &>/dev/null
    echo -e "$PASSWD\n$PASSWD" | (passwd $USERNAME) 2>/dev/null
	/usr/local/opendomo/setMailTo.sh $EMAIL "Your account in OpenDomoOS has been activated. Go to [http://$SYSTEMIP] and enter with your e-mail and password 'opendomo' to configure your account."
    manageUsers.sh
	
# With user but without passwd, see modify interface
elif ! test -z $1 && test -z $2; then
    # Extract username and email
	USERNAME=`echo $2| cut -f1 -d@`
    USERINFO=`grep ^$USERNAME /etc/passwd | awk -F: '{print$5}'`
    FULLNAME=`echo $USERINFO | cut -f1 -d"<"`
    EMAIL=`echo $USERINFO | cut -f2 -d"<" | cut -f1 -d">"`

    echo "#> Modify user"
    echo "form:`basename $0`"
    echo "	user	User	hidden	$USERNAME"
    echo "	fullname	Full name	text	$FULLNAME"
    echo "	email	Email	text	$EMAIL"
    echo "	passw	Password	password	$PASSWD"
    echo "actions:"
    echo "	goback	Back"
    echo "	userModify.sh	Modify"
    echo "	userDelete.sh	Delete"
    echo

# With passwd and user exist, modify user
elif [ `grep -c1 ^$1: /etc/passwd` -eq 1 ] && ! test -z $4; then
	USERNAME="$1"
	FULLNAME="$2"
	EMAIL="$3"
	PASSWD="$4"
    sudo usermod -c "$FULLNAME <$EMAIL>" $USERNAME  &>/dev/null
    echo -e "$PASSWD\n$PASSWD" | (passwd $USERNAME) 2>/dev/null
    manageUsers.sh

# With passwd and user don't exist, add user with password
elif [ `grep -c1 ^$1: /etc/passwd` -eq 0 ] && ! test -z $4; then
	USERNAME="$1"
	FULLNAME="$2"
	EMAIL="$3"
	PASSWD="$4"
    sudo useradd -s /bin/bash -c "$FULLNAME <$EMAIL>" -g $GROUPUID -G $USERGROUPS -m $USERNAME &>/dev/null
    echo -e "$PASSWD\n$PASSWD" | (passwd $USERNAME) 2>/dev/null
    manageUsers.sh
fi
