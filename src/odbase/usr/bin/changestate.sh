#!/bin/sh
#desc: Opendomo system states manager

# Variables
DAEMONSDIR="/usr/local/opendomo/daemons"
CONFIGSDIR="/etc/opendomo/states"
INITDIR="/etc/init.d"
STATEPID="/var/opendomo/run/states.pid"
BLACKLIST="motd bootlogs httpd mountall-bootclean.sh mountall.sh mtab.sh checkfs.sh checkroot-bootclean.sh checkroot.sh mountnfs-bootclean.sh mountnfs.sh plymouth-log plymouth hwclock.sh"

# Checks
if test -z $1; then
    echo "#ERR You need select a option [change / service]"
    exit 1
fi

daemons_blacklist () {
    cd $DAEMONSDIR
    for blackdaemon in *; do
        # Only check links
        if test -h $blackdaemon; then
            # If script is in blacklist, can be used by plugins, so clean
            for script in $BLACKLIST; do
                if [ "$blackdaemon" = "$script" ]; then
                    echo "#ERR Service $blackdaemon can be used by plugins, cleaning ..."
                        rm $blackdaemon
                fi
            done

            # Stop autostart system service survivors
            if test -h $blackdaemon; then
                insserv -r $blackdaemon 2>/dev/null
            fi
        fi
    done
}

change_services () {
    cd $DAEMONSDIR
    for daemon in *; do
        # Ignoring boot services
        if test -h "$DAEMONSDIR/$daemon" && test -x "$INITDIR/$daemon"; then
            RUNLEVEL=`cat $daemon | grep "# Default-Start:" | awk '{print $3}'`
        else
            RUNLEVEL="2"
        fi
        if [ "$RUNLEVEL" != "S" ]; then
            # Start/Stop service in state
            if ! test -h $DAEMONSDIR/$daemon && test -f $STATEDIR/$daemon; then
                # This is a opendomo service, started with admin user
                if ! $DAEMONSDIR/$daemon status &>/dev/null; then
                    sudo -u admin $DAEMONSDIR/$daemon start
                fi
            elif test -h $DAEMONSDIR/$daemon && test -f $STATEDIR/$daemon; then
                # This is a system service controled by plugins
                if ! service $daemon status &>/dev/null; then
                    service $daemon start
                fi
            elif ! test -f $STATEDIR/$daemon && test -h $DAEMONSDIR/$daemon; then
                # This is a system service stoped in this state
                if service $daemon status &>/dev/null; then
                    service $daemon stop
                fi
            else
                # This is a opendomo service stoped in this state
                if $DAEMONSDIR/$daemon status &>/dev/null; then
                    sudo -u admin $DAEMONSDIR/$daemon stop
                fi
            fi
        fi
    done
}

case $1 in
    blacklist )
        daemons_blacklist
    ;;
    change )
        STATEDIR=$CONFIGSDIR/$2

        # Check state
        if ! test -d $STATEDIR; then
            echo "#ERR State don't exist"
            exit 1
        else
            # Save new state in PID file
            echo "$2" >$STATEPID
            chown admin $STATEPID

            # Check blacklist and change services
            daemons_blacklist
            change_services
        fi
    ;;
    service )
        # Check service
        if test -h $DAEMONSDIR/$2; then
	    daemons_blacklist
	    # This a system service, check blacklist and change state
            if [ "$3" = "on" ]; then
                service $2 start
            elif [ "$3" = "off" ]; then
                service $2 stop
            else
                echo "#ERR $2 is not a valid parameter"
            fi

        elif test -f $DAEMONSDIR/$2; then
	# This a opendomo service, start with admin user
	    if [ "$3" = "on" ]; then
                sudo -u admin $DAEMONSDIR/$2 start
            elif [ "$3" = "off" ]; then
                sudo -u admin $DAEMONSDIR/$2 stop
	    else
                echo "#ERR $2 is not a valid parameter"
            fi
        fi
    ;;
esac
