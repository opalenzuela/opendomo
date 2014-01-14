#!/bin/sh
#desc:Manage tags
#type:local
#package:odcommon

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later


TAGDIR=/etc/opendomo/tags
if test -f $TAGDIR; then
	echo "#WARN Old format detected"
	mv $TAGDIR $TAGDIR.old
	mkdir $TAGDIR
	for t in `cat $TAGDIR.old`; do
		echo $t > $TAGDIR/$t
	done
fi

if ! test -d $TAGDIR; then
	mkdir $TAGDIR
fi


echo "#> Tag management"
echo "list:`basename $0`	selectable"

EXISTS=0
cd $TAGDIR
for tag in *; do
	if test $tag != "*"; then
		tname=`cat $tag`
		echo "	$tag	$tname	tag"
		EXISTS=1
	fi
done

if test "$EXISTS" = "0" ; then
	echo "# There are no tags. Please, go to Add."
	echo "actions:"
	echo "	addTag.sh	Add tag"
	echo
	exit 0
fi

echo "actions:"
echo "	addTag.sh	Add"
echo "	delTag.sh	Delete"
echo
