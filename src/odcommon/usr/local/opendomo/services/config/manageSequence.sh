#!/bin/sh
#desc:Manage sequences
#type:local
#package:odcommon

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

SEQPATH="/etc/opendomo/sequences"


if ! test -d "$SEQPATH"; then
	mkdir -p $SEQPATH
	echo "#!/bin/sh
#desc:Demo
setport.sh dummy/do01 on
wait.sh 10
setport.sh dummy/do01 off"> $SEQPATH/demo.seq
	chown admin:admin -R $SEQPATH 2>/dev/null
	chmod +x $SEQPATH/demo.seq
fi

# Display a list of sequences
cd $SEQPATH
echo "#> Available"
echo "list:manageSequenceSteps.sh	selectable"
EXIST=0
for s in *; do
	if test "$s" != "*"; then
		desc=`grep '#desc' $s | cut -f2- -d:`
		found=`grep "command:$s" /etc/opendomo/rules/* | wc -l` 2>/dev/null
		if test "$found" = "0"; then
			echo "	-$s	$desc	sequence"
			EXIST=1;
		else
			echo "	-$s	$desc	sequence used"
			EXIST=1;
		fi
	fi
done

if test "$EXIST" = "0" ]; then
	if test -x /usr/local/opendomo/addSequence.sh; then
		echo "# There are no sequences. Please, go to Add."
		echo "actions:"
		echo "	addSequence.sh	Add"
	else
		echo "# There are no sequences. Please ask your installer."
	fi
	echo
	exit 0
fi
		
echo "actions:"
if test -x /usr/local/opendomo/addSequence.sh; then
	echo "	addSequence.sh	Add"
	echo "	delSequence.sh	Delete"
fi
echo "	launchSequence.sh	Launch sequence"
echo


