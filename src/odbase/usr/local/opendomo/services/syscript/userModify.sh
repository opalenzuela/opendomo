#!/bin/sh
#desc:Manage users
#type:multiple
#group:users
#package:odbase

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

USERNAME="$1"
EMAIL="$2"
FULLNAME="$3"
PASSWD="$4"

PARAMCOUNT=$#
GROUPUID="100"
USERGROUPS="disk,audio,video"

if test "$USER" != "admin"; then
	USERNAME="$USER"
	test $# = "0" && PARAMCOUNT=1
fi


case $PARAMCOUNT in	
	0) # Without params, display "Add user" interface (only name and mail required)
#if test -z "$USERNAME"; then
		echo "#> Add user"
		echo "form:`basename $0`"
		#echo "	user	User	text	$USERNAME"
		echo "	fullname	Full name	text $FULLNAME"
		echo "	email	Email	text	$EMAIL"
		#echo "	passw	Password	password	$PASSWD"
		echo "actions:"
		echo "	goback	Back"
		echo "	userModify.sh	Add"
	;;
	1) # With only one parameter (USERNAME), see modify interface of that user
		USERINFO=`grep ^$USERNAME: /etc/passwd | awk -F: '{print$5}'`
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
	;;
	2) # Only 2 parameters (USERNAME + MAIL): creating new user with default password
		PASSWD="opendomo"
		
		sudo useradd -s /bin/bash -c "$USERNAME <$EMAIL>" -g $GROUPUID -G $USERGROUPS -m $USERNAME &>/dev/null
		echo -e "$PASSWD\n$PASSWD" | (sudo passwd $USERNAME) 2>/dev/null
		/usr/local/opendomo/bin/sendMailTo.sh $EMAIL "Your account in OpenDomoOS has been activated. Go to [http://$SYSTEMIP] and enter with your e-mail and password 'opendomo' to configure your account."
		manageUsers.sh
	;;
	3)
		#ERROR!
		echo "#ERR Invalid parameter combination"		
	;;
	4) # All 4 parameters: USER, MAIL, NAME and PASSWD (has to be yourself!!)
		if [ `grep -c1 ^$USERNAME: /etc/passwd` -eq 1 ] ; then
			# User exists. Modify
			#sudo usermod -c "$FULLNAME <$EMAIL>" $USERNAME  &>/dev/null
			echo "$EMAIL" > /home/$USERNAME/.email
			echo -e "$PASSWD\n$PASSWD" | (passwd $USERNAME) 2>/dev/null
			manageUsers.sh
		else
			echo "#ERR User does not exist"
			# User does not exist. ERROR
		fi
	
# With passwd and user exist, modify user
#elif [ `grep -c1 ^$USERNAME: /etc/passwd` -eq 1 ] && ! test -z $PASSWD; then
#    sudo usermod -c "$FULLNAME <$EMAIL>" $USERNAME  &>/dev/null
#    echo -e "$PASSWD\n$PASSWD" | (sudo passwd $USERNAME) 2>/dev/null
#    manageUsers.sh
#
# With passwd and user don't exist, add user with password
#elif [ `grep -c1 ^$USERNAME: /etc/passwd` -eq 0 ] && ! test -z $PASSWD; then
#    sudo useradd -s /bin/bash -c "$FULLNAME <$EMAIL>" -g $GROUPUID -G $USERGROUPS -m $USERNAME &>/dev/null
#    echo -e "$PASSWD\n$PASSWD" | (sudo passwd $USERNAME) 2>/dev/null
#    manageUsers.sh
#fi
	;;
esac
echo 
