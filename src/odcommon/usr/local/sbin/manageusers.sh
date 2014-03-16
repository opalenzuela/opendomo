#!/bin/sh
#desc: Manage users in opendomo

# Variables
GROUPUID="100"
USERGROUPS="disk,audio,video"
CONFIGDIR="/etc/opendomo/udata"

help () {
cat <<- HELP

  USAGE: $basename $0
      manageusers.sh  add     "user" "full name" "email" "password"
      manageusers.sh  del     "user"
      manageusers.sh  mod     "user" "full name" "email" "password"
      manageusers.sh  passwd  "user" "newpassword"

HELP
}

ch_password () {
    echo -e "$2\n$2" | (passwd $1) 2>/dev/null
}

case $1 in
    add )
	if ! test -z $5; then
            USER=$2
            FULLNAME="$3"
            EMAIL="$4"
            PASSWD=$5

            # Search correct UID
            let LASTUID=`grep "/home" "/etc/passwd" | tail -n1 | cut -f3 -d:`
            UUID=`expr $LASTUID + 1`

            # Adding user & change password
            echo "#INFO User $USER added"
	    if useradd -g $GROUPUID -G $USERGROUPS -m -u $UUID $USER; then
                ch_password $USER $PASSWD
            else
                echo "#ERRO User $USER can be added, already exist"
            fi

            # Adding extra info
            mkdir -p $CONFIGDIR
            echo "EMAIL=\"$EMAIL\""        > $CONFIGDIR/$USER.info
            echo "FULLNAME=\"$FULLNAME\"" >> $CONFIGDIR/$USER.info
            chown admin:admin $CONFIGDIR/*.info
        else
            help
        fi
    ;;
    del )
	if ! test -z $2; then
            # Checking user
            USER="$2"
            if test -z `cat "/etc/passwd" | cut -f1 -d: | grep $USER`; then
                echo "#ERRO User $USER don't exist"
                exit 1
            fi

            # Checking GID, only group users can be deleted
            let UGID=`grep $USER /etc/passwd | cut -f4 -d:`
            if [ $UGID = "100" ]; then
                echo "#INFO Deleting $USER user"
                userdel -r $USER 2>/dev/null
                rm $CONFIGDIR/$USER.info 2>/dev/null
            else
                echo "#ERRO User $USER can be deleted"
		exit 1
            fi
        else
            help
        fi
    ;;
    mod )
	if ! test -z $5; then
            USER=$2
            FULLNAME=$3
            EMAIL=$4
            PASSWD=$5

            # Checking GID, only group users can be deleted
            let UGID=`grep $USER /etc/passwd | cut -f4 -d:`
            if [ $UGID = "100" ] || [ $USER = "admin" ]; then
                echo "#WARN user $USER modified"
                echo "EMAIL=\"$EMAIL\""        > $CONFIGDIR/$USER.info
                echo "FULLNAME=\"$FULLNAME\"" >> $CONFIGDIR/$USER.info
                ch_password $USER $PASSWD
                chown admin:admin $CONFIGDIR/*.info
            else
                echo "#ERROR User $USER can be modified"
		exit 1
            fi


        else
            help
        fi
    ;;
    passwd )
	if ! test -z $3; then
            USER=$2
            PASSWD=$3
            ch_password $USER $PASSWD
        else
            help
        fi
    ;;
    * )
        help
    ;;
esac
