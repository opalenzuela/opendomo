#!/bin/sh
#desc:Manage users
#type:multiple
#group:users
#package:odbase

# Copyright(c) 2015 OpenDomo Services SL. Licensed under GPL v3 or later

### DEPRECATED SCRIPT:  use editUser.sh instead ###

USERNAME="$1"
EMAIL="$2"
FULLNAME="$3"
PASSWD="$5"

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
		echo "	fullname	Full name	text $FULLNAME"
		echo "	email	Email	text	$EMAIL"
		echo "actions:"
		echo "	goback	Back"
		echo "	userModify.sh	Add"
	;;
	1) # With only one parameter (USERNAME), see modify interface of that user
		USERINFO=`grep ^$USERNAME: /etc/passwd | awk -F: '{print$5}'`
		FULLNAME=`echo $USERINFO | cut -f1 -d"<"`
		EMAIL=`echo $USERINFO | cut -f2 -d"<" | cut -f1 -d">"`
		if test -f /home/$USERNAME/.fullname; then
			FULLNAME=`cat /home/$USERNAME/.fullname`
		fi
		
		echo "#> Modify user"
		echo "form:`basename $0`"
		echo "	user	User	hidden	$USERNAME"
		echo "	email	Email	text	$EMAIL"
		echo "	fullname	Full name	text	$FULLNAME"		
		echo "	passwordsep	Change password	separator"
		echo "	opassw	Previous	password hidden	"		
		echo "	npassw	Password	password hidden	"
		echo "	passw	Re-type 	password hidden	"
		echo "actions:"
		echo "	goback	Back"
		echo "	userModify.sh	Save"
		if test -x /usr/local/opendomo/userDelete.sh; then
			echo "	userDelete.sh	Delete"
		fi
	;;
	2) # Only 2 parameters (USERNAME + MAIL): creating new user with default password
		PASSWD="opendomo"
		
		sudo useradd -s /bin/bash -c "$USERNAME <$EMAIL>" -g $GROUPUID -G $USERGROUPS -m $USERNAME &>/dev/null
		echo -e "$PASSWD\n$PASSWD" | (sudo passwd $USERNAME) 
		SUBJECT="Your account in OpenDomoOS has been activated"
		MESSAGE="Your account in OpenDomoOS has been activated. Follow this link and enter with your e-mail and password opendomo to configure your account."
		#TODO Use final IP instead of autoconf
		SYSTEMIP="169.254.0.25"
		LINK="http://$SYSTEMIP/"
		/usr/local/opendomo/bin/sendMailTo.sh "$EMAIL" "$SUBJECT" "$MESSAGE" "$LINK"
		manageUsers.sh
	;;
	3)
		#ERROR!
		echo "#ERR Invalid parameter combination"		
	;;
	*)# All parameters: USER, MAIL, NAME and PASSWD (has to be yourself!!)
		if [ `grep -c1 ^$USERNAME: /etc/passwd` -eq 1 ] ; then
			OLDPASS="$4"
			if test "$5" != "$6"; then
				echo "#ERR Entered passwords don't match"
				exit 1
			else
				#sudo usermod -c "$FULLNAME <$EMAIL>" $USERNAME  &>/dev/null
				if touch /home/$USERNAME/.email; then
					echo "$EMAIL" > /home/$USERNAME/.email
					echo "$FULLNAME" > /home/$USERNAME/.fullname
					if test -z "$OLDPASS" || test -z "$PASSWD" ; then
						echo "$FULLNAME" > /home/$USERNAME/.fullname
					else
						echo -e "$OLDPASS\n$PASSWD\n$PASSWD" | (passwd $USERNAME) 2>/dev/null
					fi
					if test -x /usr/local/opendomo/manageUsers.sh; then
						/usr/local/opendomo/manageUsers.sh
					else
						echo "#INFO Configuration successfully modified"
					fi
				else
					echo "#WARN Missing privileges"
				fi
			fi
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
