#!/bin/sh
#desc:Save configuration
#package:odcommon
#type:multiple

echo "#> Saving data"
echo "list:`basename $0`"
echo "#WARN This actions modify system permanently and can't be undo"
echo "actions:"
echo "	saveConfig.sh	Save configuration"
echo "	saveConfigReboot.sh	Save & reboot"
echo "	loadDefaultConfig.sh	Load default configuration"
echo "	restoreDefaultSystem.sh	Restore default system"
echo
