#!/bin/sh
#desc:Process rules
#type:local
#package:odcommon
RULESDIR="/etc/opendomo/rules"
BINDIR="/usr/local/bin"
PORTDIR="/var/opendomo/control"

mkdir -p $RULESDIR
cd $RULESDIR
for i in *; do
	if test "$i" != "*" ; then
		echo "Processing $i"
#		/bin/logevent notice odcommon "Processing $i"
		if ! test -x $RULESDIR/$i; then
			chmod +x $RULESDIR/$i
		fi
		export PATH="/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/opendomo:/etc/opendomo/sequences"
		if $RULESDIR/$i; then
			echo "    (fulfilled!)"				
			COMMAND=`grep '#command' $i | cut -f2 -d:`
			BIN=`echo $COMMAND | cut -f1 -d' '`
			/bin/logevent notice odcommon "Condition [$i] is TRUE"
			if test -f /var/run/$i-cond.pid; then
				/bin/logevent debug odcommon "Avoiding repeated [$COMMAND]"
			else
				touch /var/run/$i-cond.pid
				/bin/logevent notice odcommon "Executing [$COMMAND]"
				$COMMAND 
			fi
		else
			rm -fr /var/run/$i-cond.pid 
			echo "    (not fulfilled)"
		fi
	fi
done
