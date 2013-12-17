if [ `whoami` = admin ]; then
	echo "#WARN Rebooting now ..."
	sudo shutdown -r now
else
	echo "#ERR Only admin can reboot"
fi
