#!/bin/sh
#desc:Save configuration
#package:odcommon
#type:multiple

echo "#> Saving data"
echo "list:`basename $0`"
echo "#WARN This actions modify system permanently and can't be undo"
echo "actions:"
echo "	saveSystemConfig.sh	Save configuration"
echo "	loadDefaultConfig.sh	Load default configuration"
echo "	restoreDefaultSystem.sh	Restore default system"
echo "	reboot.sh	Reboot system"
echo
