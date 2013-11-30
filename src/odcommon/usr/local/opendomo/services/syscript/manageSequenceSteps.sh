#!/bin/sh
#desc:Manage sequence steeps
#type:local
#package:odcommon

# Copyright(c) 2011 OpenDomo Services SL. Licensed under GPL v3 or later

SEQPATH="/etc/opendomo/sequences"


if ! test -d "$SEQPATH"; then
	mkdir -p $SEQPATH
fi

# List of all supported scripts in /usr/local/bin
POSSIBLECOMMANDS="setport.sh setallports.sh wait.sh play.sh"
for c in $POSSIBLECOMMANDS; do
	# If they exist, and can be executed
	if test -x /usr/local/bin/$c; then
		desc=`grep '#desc' /usr/local/bin/$c | cut -f2 -d:` 2>/dev/null
		if test -z "$desc"; then
			desc="$c"
		fi
		# We add them to the list
		if test -z "$COMMANDS"; then
			COMMANDS="$c:$desc"
		else
			COMMANDS="$COMMANDS,$c:$desc"
		fi
	fi
done

# Is odcontrol properly installed + configured? 
if test -d /etc/opendomo/control; then
	ARGS1="@setport.sh"
	cd /etc/opendomo/control
	for i in `grep way -r * | grep out | cut -f1 -d.`; do
		if test -f $i.desc; then
			desc=`cat $i.desc 2>/dev/null `
		else
			desc=`echo $i | cut -f2 -d/`  
		fi
		if test -z "$desc"; then
			desc="$i"
		fi
		ARGS1="$ARGS1,$i:$desc"
	done
	ARGS2="@setport.sh,on,off"
fi

if ! test -d /etc/opendomo/tags; then
	/usr/local/opendomo/manageTags.sh
fi

cd /etc/opendomo/tags
ARGS1="$ARGS1,@setallports.sh"
for t in *; do
	if test "$t" != "*"; then
		ARGS1="$ARGS1,$t:$t"
	fi
done
ARGS2="$ARGS2,@setallports.sh,on,off"

ARGS1="$ARGS1,@play.sh"
# All the sounds in this directory can be called
cd /usr/share/sounds
ARGS2="$ARGS2,@play.sh,"
if test -x /usr/bin/aplay; then
	for wav in *; do
		if test $wav != "*"; then
			wavname=`echo $wav | cut -f1 -d.`
			ARGS1="$ARGS1,$wav:$wavname"
		fi
	done
fi

ARGS1="$ARGS1,@wait.sh"
ARGS2="$ARGS2,@wait.sh,"
for time in 1 2 3 4 5 6 7 8 9 10 15 20 30 60; do
	ARGS1="$ARGS1,$time:$time s"
done




# Graphical Inteface
if ! test -z "$1"; then

	if test -f "$SEQPATH/$1"; then
		# A file is selected
		FILE="$1"
		SEQDESC=`grep 'desc:' $SEQPATH/$FILE | cut -f2 -d:`
	else
		if test "$2" = "addnew"; then
			if test -z "$3"; then
				/usr/local/opendomo/addSequence.sh
				exit 0
			fi
			if test "$1" = "new"; then
				code=`echo $3 | tr A-Z a-z | sed 's/[^a-z0-9]//g'`
				if test -z "$code"; then
					echo "#ERR Invalid name"
					exit 2
				fi
			else
				code="$1"
			fi
			desc=`echo $3 | sed 's/+/ /g'`
			SEQDESC="$desc"
			FILE="$code.seq"

			if test -e "$SEQPATH/$FILE"; then
				echo "#ERR File exists"
				exit 2
			fi

			echo "#!/bin/sh
#desc:$desc"  > $SEQPATH/$FILE
			chown admin:admin $SEQPATH/$FILE 2>/dev/null
			chmod +x $SEQPATH/$FILE
		else
			echo "#ERR File not found"
		fi
	fi

	case "$2" in
		"addstep")
			CMD=`echo $command | sed 's/+/ /g'`
			echo "$CMD # $command_ $command__ $command___" >> $SEQPATH/$FILE
			echo "#INFO: Sequence step added"
		;;
	esac

	par1=""
	echo "#> Steps in [$SEQDESC]"
	echo "list:`basename $0`	selectable "
	for line in `grep -nv '^#' $SEQPATH/$FILE | sed 's/ /:/g'`; do
		lineno=`echo $line | cut -f1 -d:`  
		command=`echo $line | cut -f2 -d# | sed -e 's/:/ /g' -e 's/+/ /g'`

		echo "	$FILE-$lineno	$command	step"
	done
	if test -z "$command"; then
		echo "#INFO No steps defined yet. Select the action in the menu and press Add."
	fi
	
	echo "actions:"
	echo "	delSequenceStep.sh	Delete step"
	echo
	

	echo "#> Add new step"
	echo "form:`basename $0`	rowform"
	echo "	file	file	hidden	$FILE"
	echo "	action	action	hidden	addstep"
	echo "	command	Command	text	$CMD"
	echo "actions:"
	echo "	`basename $0`	Add"
	echo "	manageSequence.sh	Back to list"
	echo	
fi


