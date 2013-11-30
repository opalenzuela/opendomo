#!/bin/sh
echo "Content-type: text/javascript"
echo "Cache-Control: max-age=3600"
echo 
echo
echo "var option_tree = {"

if test -x /usr/local/bin/weekday
then
	literal=`/usr/local/bin/i18n.sh "weekday"`
	mon=$(/usr/local/bin/i18n.sh monday)
	tue=$(/usr/local/bin/i18n.sh tuesday)
	wed=$(/usr/local/bin/i18n.sh wednesday)
	thu=$(/usr/local/bin/i18n.sh thursday)	
	fri=$(/usr/local/bin/i18n.sh friday)	
	sat=$(/usr/local/bin/i18n.sh saturday)	
	sun=$(/usr/local/bin/i18n.sh sunday)	
	echo "		\"$literal =\": {
			'$mon':\"(weekday) = '1'\",
			'$tue':\"(weekday) = '2'\",
			'$wed':\"(weekday) = '3'\",
			'$thu':\"(weekday) = '4'\",
			'$fri':\"(weekday) = '5'\",
			'$sat':\"(weekday) = '6'\",
			'$sun':\"(weekday) = '7'\",			
			"
	echo '               }, '
fi

if test -x /usr/local/bin/hour
then
	literal=`/usr/local/bin/i18n.sh "hour"`
	echo "		\"$literal\": {"
	for i in 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 
	do
		echo "				'= $i:00':\"(hour) = '$i.00'\","
	done
	echo '               }, '
fi


#TODO Make this list dynamic
vars="weekday:weekday,hour:hour,monthday:day of the month"
VALUES="@weekday,1:monday,2:tuesday,3:wednesday,4:thursday,5:friday,6:saturday,7:sunday"
VALUES="$VALUES,@hour,00.00,01.00,02.00,03.00,04.00,05.00,06.00,07.00,08.00,09.00,10.00,11.00,12.00,13.00,14.00,15.00,16.00,17.00,18.00,19.00,20.00,21.00,22.00,23.00"
VALUES="$VALUES,@monthday,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31"

# Is odcontrol properly installed + configured? 
if test -d /etc/opendomo/control; then
	literal=`/usr/local/bin/i18n.sh "port state"`
	echo "		\"$literal\": {"	
	cd /etc/opendomo/control
	for i in `grep "way='in'" -r * | cut -f1 -d.`; do
		desc=`grep 'desc=' $i.info | cut -f2 -d=`
		pvalues=`grep 'values=' $i.info | cut -f2 -d= | sed "s/'//g"`
		if test -z "$desc"; then
			desc=`basename $i`
		fi
		if test -z "$pvalues"; then
			#TODO consider analog/digital ports
			pvalues="on,off,10,20,30,40,50,60,70,80,90,100"
		fi
		echo "
			'$desc =': {"
		pvals=$(echo $pvalues | sed 's/,/ /g')
		for v in $pvals
		do
			echo "				'$v':\"(cat /var/opendomo/control/$i) = $v\","
		done
		echo "			},"
	done
	echo '               }, '	
fi


#END OF SCRIPT
echo '    };  '
