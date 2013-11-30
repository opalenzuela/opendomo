#!/bin/sh
echo "Content-type: text/javascript"
echo 
echo
echo "var option_tree = {"

if test -x "/usr/local/bin/setport.sh"
then
	literal=`/usr/local/bin/i18n.sh "Set port"`
	echo "		\"$literal\": {"

	# Is odcontrol properly installed + configured? 
	if test -d /etc/opendomo/control
	then
		ARGS1="@setport.sh"
		cd /etc/opendomo/control
		for i in */*.info; do
			desc=""
			. $i
			if test "$way" = "out" && test "$status" = "enabled"
			then
				if test -z "$desc"
				then
					desc=`echo $i | cut -f2 -d/ | cut -f1 -d.`  
				fi
				pname=`echo $i | cut -f1 -d.`
				#TODO: This should display all possible values for each port
				echo "
				\"$desc\": {
					'ON':\"setport.sh $pname ON\",
					'OFF':\"setport.sh $pname OFF\",				
					},"
			fi
		done
	fi
	echo '               }, '
fi

if test -x "/usr/local/bin/setallports.sh"; then
	literal=`/usr/local/bin/i18n.sh "Set all"`
	echo "		\"$literal\": {"
	cd /etc/opendomo/tags
	ARGS1="$ARGS1,@setallports.sh"
	for t in *; do
		if test "$t" != "*"; then
			desc=`cat $t`
			echo "
			\"$desc\": {
				'ON':\"setallports.sh $t ON\",
				'OFF':\"setallports.sh $t OFF\",				
				},"
		fi
	done	
	echo '               },'
fi             

# All the sounds in this directory can be called
if test -x /usr/bin/aplay; then
	literal=`/usr/local/bin/i18n.sh "Play"`
	echo "		\"$literal\": {"
	cd /usr/share/sounds
	ARGS1="$ARGS1,@setallports.sh"
	for wav in *; do
		if test $wav != "*"; then
			wavname=`echo $wav | cut -f1 -d.`
			echo "'$wavname':\"play.sh $wav\","

		fi
	done
	echo '               },'
fi             

literal=`/usr/local/bin/i18n.sh "Wait"`
echo "		\"$literal\": {"
echo '			"seconds": {'
for i in `seq 1 60` ; do
	echo "				'$i s':'wait.sh $i',"
done	
echo '			}, '

echo '
			"minutes": {'
for i in `seq 1 60` ; do
	let m=$i*60
	echo "				'$i m':'wait.sh $m',"
done					
echo '			}, '

echo '
			"hours": {'
for i in `seq 1 24` ; do
	let m=$i*60*60
	echo "				'$i h':'wait.sh $m',"
done					
echo '			}, '

echo '		}, '


#END OF SCRIPT
echo '    };  '
  
