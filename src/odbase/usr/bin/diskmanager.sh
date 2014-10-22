#!/bin/sh
#desc: Daemon that seeks for new disks

PIDFILE="/var/opendomo/run/diskmanager.pid"

# Funcion to process mounted drives
process_drive () {
	# Check opendomo mounted and process drives
	cd /media
	for drive in *; do
		# Check if disk isn't processed yet
		if [ `grep -m1 -c $drive $PIDFILE` = "0" ]; then

			# Scanning drive
			if test -f $drive/opendomo.cfg; then
				for disktype in $(grep DISKTYPE $drive/opendomo.cfg | cut -f2 -d"\""); do
					# Launch events
					logevent add"$disktype"disk odcommon "$disktype disk type added" $drive
				done
			fi
			echo "$drive" >>$PIDFILE
		fi
	done
}

# Funtion to delete unmounted processed drives
delete_drive () {
	# Delete unmount processed drives
	cd /media
	for disk in *; do
                MOUNT=`ls -l $disk |  awk '{print$11}'`
		# Check changes in /proc/mount
		if [ `grep -c -m1 "$MOUNT" /proc/mounts` = "0" ]; then
			# Launch event
			logevent deldisk odcommon "disk deleted" $disk

			# Delete media link and make pid without unmounted drive
			SAVEDDRIVES=`cat $PIDFILE | grep -v "$disk"`
			echo "$SAVEDDRIVES" >$PIDFILE
			rm /media/$disk
		fi
	done
}

# Run loop while pid exist
while test -f "$PIDFILE"; do
	process_drive
	delete_drive
	sleep 1
done
