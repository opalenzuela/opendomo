#!/bin/sh
cd /
for i in `find /usr/local/opendomo/services/ -type f`
do
    bn=`basename $i`
	# Solamente creamos wrapper si no existe
    if ! test -f /usr/local/opendomo/$i; then
		# Forzamos permisos de ejecucion
		chmod +x $i
		# Creamos enlace simbólico
		ln -fs $i /usr/local/opendomo/ >/dev/null
    fi
done
# Creacion de los enlaces del CGI
ROOTPATH="/var/opendomo/cgiroot"
SCRIPTPATH="/usr/local/opendomo"
for section in config control map tools
do
    if test -d  "$SCRIPTPATH/services/$section"
    then
	cd $SCRIPTPATH/services/$section
	for i in *.sh
	do
	    mkdir -p $ROOTPATH/$section
	    ln -fs $SCRIPTPATH/$i $ROOTPATH/$section/$i
	    grep '#desc' $i | head -n1| cut -f2 -d: > $ROOTPATH/$section/$i.name
	done
    fi
done
