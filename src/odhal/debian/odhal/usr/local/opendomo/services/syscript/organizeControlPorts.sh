#!/bin/sh
#desc:Organize ports
#type:local
#package:odhal

CTRLPATH="/var/opendomo/control"
CFGPATH="/etc/opendomo/control"

BYZONEPATH="/var/opendomo/control_byzone"
BYTAGPATH="/var/opendomo/control_bytag"
ALLPATH="/var/opendomo/control_allports"

rm -fr $BYZONEPATH
rm -fr $BYTAGPATH
rm -fr $ALLPATH

mkdir $BYZONEPATH
mkdir $BYTAGPATH
mkdir $ALLPATH

cd /etc/opendomo/zones/
ZONES=`ls`

cd /etc/opendomo/tags/
TAGS=`ls`

for z in $ZONES none; do
	mkdir $BYZONEPATH/$z
done

for t in $TAGS none; do
	mkdir $BYTAGPATH/$t
done

cd $CFGPATH
for d in *; do
	if test "$d" != "*" && test -d "$d" && test -d $CTRLPATH/$d
	then
		cd $d
		
		for p in *.info; do
			if test $p != "*.info"; then
				pname=`echo $p | cut -f1 -d.`
				echo " Including $CFGPATH/$d/$p"
				zone=""
				tag=""
				. $CFGPATH/$d/$p
				if ! test -f "$CFGPATH/$d/$pname.desc"; then
					echo $p | sed 's/.info//' > $CFGPATH/$d/$pname.desc
				fi

				if test "$status" != "disabled" 
				then
					if test -z "$zone"; then
						echo "$d" > $BYZONEPATH/none/${d}_${pname}.device
						ln -s $CFGPATH/$d/$pname.info $BYZONEPATH/none/${d}_${pname}.info
						#ln -s $CFGPATH/$d/$pname.desc $BYZONEPATH/none/${d}_${pname}.desc
						ln -s $CTRLPATH/$d/$pname $BYZONEPATH/none/${d}_${pname}
						ln -s $CTRLPATH/$d/$pname.value $BYZONEPATH/none/${d}_${pname}.value
					else
						echo "$d" > $BYZONEPATH/$zone/${d}_${pname}.device
						ln -s $CFGPATH/$d/$pname.info $BYZONEPATH/$zone/${d}_${pname}.info
						#ln -s $CFGPATH/$d/$pname.desc $BYZONEPATH/$zone/${d}_${pname}.desc
						ln -s $CTRLPATH/$d/$pname $BYZONEPATH/$zone/${d}_${pname}
						ln -s $CTRLPATH/$d/$pname.value $BYZONEPATH/$zone/${d}_${pname}.value						
					fi
				
					if test -z "$tag"; then
						ln -s $CFGPATH/$d/$pname.info $BYTAGPATH/none/${d}_${pname}.info
						#ln -s $CFGPATH/$d/$pname.desc $BYTAGPATH/none/${d}_${pname}.desc
						ln -s $CTRLPATH/$d/$pname $BYTAGPATH/none/${d}_${pname}
						ln -s $CTRLPATH/$d/$pname.value $BYTAGPATH/none/${d}_${pname}.value	
					
					else
						ln -s $CFGPATH/$d/$pname.info $BYTAGPATH/$tag/${d}_${pname}.info
						#ln -s $CFGPATH/$d/$pname.desc $BYTAGPATH/$tag/${d}_${pname}.desc
						ln -s $CTRLPATH/$d/$pname $BYTAGPATH/$tag/${d}_${pname}
						ln -s $CTRLPATH/$d/$pname.value $BYTAGPATH/$tag/${d}_${pname}.value	
					fi
					ln -s $CTRLPATH/$d/$pname $ALLPATH/${d}_${pname}
					touch $CTRLPATH/$d/$pname.value
					ln -s $CTRLPATH/$d/$pname.value $ALLPATH/.${d}_${pname}.value
					ln -s $CFGPATH/$d/$pname.info $ALLPATH/.${d}_${pname}.info					
				else
					echo " Port $p disabled"
				fi
				
			fi
		done
		
		cd ..
	fi
done
