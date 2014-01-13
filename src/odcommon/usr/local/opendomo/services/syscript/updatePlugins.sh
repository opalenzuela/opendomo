#!/bin/sh
#type:local
#package:odcommon

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

# Required in order to update the repository index file
/usr/local/opendomo/managePlugins.sh
# Calling the actual update manager (with the compulsory parameter)
/usr/local/bin/plugin_update.sh now

echo