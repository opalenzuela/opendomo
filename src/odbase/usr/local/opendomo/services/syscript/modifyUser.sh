#!/bin/sh
#desc:Manage users
#type:multiple
#package:odbase

CONFIGDIR="/etc/opendomo/udata"
USER="$1"
FULLNAME="$2"
EMAIL="$3"
PASSWD="$4"

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
    echo "	modifyUser.sh	Add"
    echo

# With user but without passwd, see modify interface
elif ! test -z $USER && test -z $PASSWD; then
    if test -f $CONFIGDIR/$USER.info; then
        FULLNAME=`grep ^FULLNAME= $CONFIGDIR/$USER.info | sed 's/\"//g' | cut -f2 -d= `
        EMAIL=`grep ^EMAIL= $CONFIGDIR/$USER.info | sed 's/\"//g' | cut -f2 -d= `
    fi
    echo "#> Modify user"
    echo "form:`basename $0`"
    echo "	user	User	hidden	$USER"
    echo "	fullname	Full name	text	$FULLNAME"
    echo "	email	Email	text	$EMAIL"
    echo "	passw	Password	password	$PASSWD"
    echo "actions:"
    echo "	goback	Back"
    echo "	modifyUser.sh	Modify"
    echo "	deleteUser.sh	Delete"
    echo

# With passwd and user exist, modify user
elif ! test -z `cat "/etc/passwd" | cut -f1 -d: | grep $USER` && ! test -z $PASSWD; then
    sudo manageusers.sh mod "$USER" "$FULLNAME" "$EMAIL" "$PASSWD"
    manageUsers.sh

# With passwd and user don't exist, add user
elif test -z `cat "/etc/passwd" | cut -f1 -d: | grep $USER` && ! test -z $PASSWD; then
    sudo manageusers.sh add "$USER" "$FULLNAME" "$EMAIL" "$PASSWD"
    manageUsers.sh
fi
