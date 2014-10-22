#!/bin/sh
#desc:Genera una tabla de contenidos de toda la interfaz OpenDomo
#auth:opalenzuela
#
# El script TOC realizará un barrido  por toda la estructura de directorios de
# /etc/opendomo/root mostrando todos los elementos disponibles para el usuario
# validado. Esta operación será realizada solo una vez por sesión iniciada, de
# modo que el número de scripts llamados por el cgi se minimizará, optimizando
# los tiempos de carga de cada página.

RAIZ="/cgi-bin/od.cgi"
CFGDIR="/var/opendomo/cgiroot/"
cd $CFGDIR
# Solamente mostramos los enlaces con extensión ".sh", evitando directorios y
# enlaces a archivos .css o .js

echo "toc:`basename $0`"
for file in find -type l -name "*.sh"
do
	description=`cat $file.name`
	echo "	$file	$description	toc"
done
echo
