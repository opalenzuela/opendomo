#!/bin/sh
#desc:Save system configuration
#package:odbase
#type:multiple


echo '{"status":"busy"}' > /var/www/data/status.json
sudo mkrootfs
echo '{"status":"active"}' > /var/www/data/status.json

echo "#> Configuration saved"
echo "#INFO Your configuration was saved"
echo
echo "actions:"
echo "	reboot.sh	Reboot now"
echo
