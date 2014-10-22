#!/bin/sh
#desc:Update language files
#package:odbase
#type:local

# Copyright(c) 2014 OpenDomo Services SL. Licensed under GPL v3 or later

LPATH="/var/opendomo/i18n"
CFGPATH="/etc/opendomo/langfiles/"
URLTRANS="http://cloud.opendomo.com/babel/trans.php"
LANGFILE="/etc/opendomo/i18n/lang"
SLANG=`cat $LANGFILE`

mkdir -p $CFGPATH

#cd /var/pkg/installed
#PACKAGES=`echo * | sed 's/ /+/g'`
PACKAGES="all"
echo "#LOADING Updating language files"
echo
cd $CFGPATH
if wget -q "$URLTRANS?modules=$PACKAGES&lang=key" -O $CFGPATH/key  && \
	wget -q "$URLTRANS?modules=$PACKAGES&lang=$SLANG" -O $CFGPATH/$SLANG 
then
	cp $CFGPATH/$SLANG $CFGPATH/key $LPATH/
	chown admin $CFGPATH/* 2>/dev/null
	chmod 0766 $CFGPATH/* 2> /dev/null
	echo "#INFO Translation files updated"
	kcount=`cat $CFGPATH/key | wc -l`
	lcount=`cat $CFGPATH/$SLANG | wc -l`
	if test "$kcount" -gt "$lcount"; then
		echo "#WARN The translation files for this language are not complete"
		echo "# You can find more information in the following page"
		echo "#URL http://es.opendomo.org/wiki/index.php?title=Babel"
		echo
	fi
else
	echo "#INFO Community files not available"
fi
wget -q "$URLTRANS?lang=all" -O $CFGPATH/available 
chown admin $LPATH/* 2>/dev/null
echo
