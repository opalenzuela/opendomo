#!/bin/sh
cd /
for i in `find /usr/local/opendomo/services/ -type f`
do 
    bn=`basename $i`
    if ! test -f /usr/local/opendomo/$i; then
       # Solamente creamos wrapper si no existe
       ln -fs $i /usr/local/opendomo/ >/dev/null
    fi
done
# Creacion de los enlaces del CGI 
ROOTPATH="/var/opendomo/cgiroot"
SCRIPTPATH="/usr/local/opendomo"
for section in config control map tools
do
   cd $SCRIPTPATH/services/$section
   for i in *.sh 
   do
       mkdir -p $ROOTPATH/$section
       ln -fs $SCRIPTPATH/$i $ROOTPATH/$section/$i
       grep '#desc' $i | head -n1| cut -f2 -d: > $ROOTPATH/$section/$i.name
   done
done
