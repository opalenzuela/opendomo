#!/bin/sh
#desc: Carga la configuración
#package: odcommon
#type: local

DEFFILE="/mnt/odconf/pdefault/sysconf.tar.gz"
ENC="/mnt/sysback/cfgbackup.enc"
LOGFILE="/var/log/loadcfg.log"

if test -z "$1"; then
	CFGFILE="/mnt/odconf/sysconf/sysconf.tar.gz"
else
	CFGFILE="$1"
fi

if test -f "$CFGFILE"; then
	echo "# Loading compressed configuration ..." | tee $LOGFILE
	if gunzip -c $CFGFILE | tar -xv -C / >> $LOGFILE
	then
		echo -n "OK"
	fi
else
	echo "#ERR Configuration file [$CFGFILE] not found" | tee $LOGFILE
fi

if test -f "$DEFFILE" && diff "$CFGFILE.gz" "$DEFFILE"; then
	echo "#WARN Loading default configuration" | tee $LOGFILE
	# Archivo de configuración y configuración por defecto son iguales:
	# Es un sistema no configurado
	if test -f $ENC; then
		echo "# Encrypted backup found! Setting up restore wizard" | tee $LOGFILE
		# Mientras exista el archivo de configuración cifrado
		cd /var/www/cgi-bin/
		# Desactivamos el CGI
		mv od.cgi od.cgi.bak
		chmod +x loadconf.cgi
		# Lo reemplazamos por el cargador de configuración
		ln -s loadconf.cgi od.cgi
	fi
fi
