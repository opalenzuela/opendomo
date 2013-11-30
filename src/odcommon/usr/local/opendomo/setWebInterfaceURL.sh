#!/bin/sh
#package: distro
#desc: Asigna la URL del servicio CGI disponible en la red

if test -z "$1"; then
	echo "#ERR: You must specify the URL"
else
	URL=`echo $1 | sed -e 's/\//%2F%/g'` 
	cat /var/www/index.html.default | sed -e "s/%URL%/$URL/g" -e "s/%2F%/\//g" > /var/www/index.html
fi
