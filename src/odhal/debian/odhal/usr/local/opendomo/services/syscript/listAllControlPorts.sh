#!/bin/sh
#desc:List all control ports
#package:odhal
#type:local

ALLPATH="/var/opendomo/control_allports"

if ! test -d $ALLPATH; then
	/usr/local/opendomo/services/syscript/organizeControlPorts.sh
fi

echo "form:`basename $0`"
cd $ALLPATH
for p in *; do
	#. .$p.info
	value=""
	if test -f .$p.value; then
		value=$(cat .$p.value)
	fi
	test -z "$value" && value="-"
	echo "	$p	$p	readonly	$value"
done
echo
