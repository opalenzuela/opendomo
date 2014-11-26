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

# Without params, see add interface
if test -z $USER; then
    echo "#> Add user"
    echo "form:`basename $0`"
    echo "	user	User	text	$USER"
    echo "	fullname	Full name	text $FULLNAME"
    echo "	email	Email	text	$EMAIL"
    echo "	passw	Password	password	$PASSWD"
    echo "actions:"
    echo "	goback	Back"
    echo "	userModify.sh	Add"
    echo

# With user but without passwd, see modify interface
elif ! test -z $USER && test -z $PASSWD; then
    # Extract username and email
    USERINFO=`grep $USER: /etc/passwd | awk -F: '{print$5}'`
    FULLNAME=`echo $USERINFO | cut -f1 -d"<"`
    EMAIL=`echo $USERINFO | cut -f2 -d"<" | cut -f1 -d">"`

    echo "#> Modify user"
    echo "form:`basename $0`"
    echo "	user	User	hidden	$USER"
    echo "	fullname	Full name	text	$FULLNAME"
    echo "	email	Email	text	$EMAIL"
    echo "	passw	Password	password	$PASSWD"
    echo "actions:"
    echo "	goback	Back"
    echo "	userModify.sh	Modify"
    echo "	userDelete.sh	Delete"
    echo

# With passwd and user exist, modify user
elif [ `grep -c1 $USER: /etc/passwd` -eq 1 ] && ! test -z $PASSWD; then
    sudo usermod -c "$FULLNAME <$EMAIL>" $USER  &>/dev/null
    echo -e "$PASSWD\n$PASSWD" | (passwd $USER) 2>/dev/null
    manageUsers.sh

# With passwd and user don't exist, add user
elif [ `grep -c1 $USER: /etc/passwd` -eq 0 ] && ! test -z $PASSWD; then
    sudo useradd -s /bin/bash -c "$FULLNAME <$EMAIL>" -g $GROUPUID -G $USERGROUPS -m $USER &>/dev/null
    echo -e "$PASSWD\n$PASSWD" | (passwd $USER) 2>/dev/null
    manageUsers.sh
fi
