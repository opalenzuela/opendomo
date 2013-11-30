#!/bin/sh
#package:odcgi
#desc:Configure interface
#type:local

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

SKINFILE="/etc/opendomo/cgi_skin"
CFGFILE="/etc/opendomo/cgi_style"
I18N_PATH="/var/opendomo/i18n"
CFG_PATH="/etc/opendomo/i18n"


# ====== SAVE CONFIG IF ARGS PASSED ==========
if test -n "$3"; then
	if test -d /var/www/themes/$1; then
		if touch $CFGFILE
		then
			echo "$1" > $CFGFILE
		else
			echo "#ERR Missing privileges"
		fi

	else
		echo "#ERR Theme [$1] does not exist"
		exit 1
	fi
	if test -d /var/www/skins/$2; then
		if touch $SKINFILE; then
			echo "$2" > $SKINFILE
		else
			echo "#ERR Missing privileges"
		fi
	else
		echo "#ERR Skin [$2] does not exist"
		exit 2
	fi
		
	echo $3 > $CFG_PATH/lang
	chown admin $CFG_PATH/lang 2>/dev/null 
	/usr/local/opendomo/updateLanguageFiles.sh
fi

# ====== OBTAIN SELECTED VALUES ===============
if test -f $CFGFILE; then
	SELSTYLE=`cat $CFGFILE` 
else
	SELSTYLE="default"
fi

if test -f $SKINFILE; then
	SELSKIN=`cat $SKINFILE` 
else
	SELSKIN="silver"
fi

if test -e $CFG_PATH/lang
then
	SELECTED=`cat $CFG_PATH/lang`
else
	SELECTED="es"
fi

# ======= CREATE LISTS OF VALID VALUES ========
for i in /var/www/skins/*; do
	this=`basename $i`
	if test -z "$SKINS"; then
		SKINS="$this:$this"
	else
		SKINS="$SKINS,$this:$this"
	fi
done

for i in /var/www/themes/*; do
	if test -d $i; then
		this=`basename $i`
		if test -z "$THEMES"; then
			THEMES="$this:$this"
		else
			THEMES="$THEMES,$this:$this"
		fi
	fi
done

if ! test -f /etc/opendomo/langfiles/available; then
	wget -q "http://www.opendomo.org/trans.php?lang=all" -O /etc/opendomo/langfiles/available
fi
if test -f /etc/opendomo/langfiles/available; then
	LANG_LIST=`cat /etc/opendomo/langfiles/available`
fi

echo "#>Configure interface"
echo "form:`basename $0`"
echo "	theme	Theme	list[$THEMES]	$SELSTYLE"
echo "	skin	Skin	list[$SKINS]	$SELSKIN"
echo "	lang	Language	list[$LANG_LIST]	$SELECTED"
echo
