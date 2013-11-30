#!/bin/sh
echo "Content-type: text/css"
echo "Cache-Control: max-age=3600"
echo
CFGPATH="/etc/opendomo/zones"

if test -f /etc/opendomo/map.conf
then
	. /etc/opendomo/map.conf
else
	echo "/* No map file found */"
	exit 1
fi

if ! test -d $CFGPATH
then
	echo "/* No configuration file found */"
	exit 1
fi

if test -z "$FILE"; then
	# File not specified (old config??)
	if test -z "$URL"; then
		echo "/* No map available */"	
		exit 0
	fi
else
	# File specified and existent...
	if test -f "/media/$FILE"; then
		# ... se we build the URL
		URL="/cgi-bin/od.cgi/tools/browseFiles.sh?FILE=$FILE&GUI=none"
	else
		# If it does not exist, abort. No map available
		echo "/* No map file available: $FILE */"			
		exit 0
	fi
fi

echo "
.map ul {
	background-image:url('$URL');
	background-repeat: no-repeat;
	height: ${HEIGHT}px;
	width: ${WIDTH}px;
	display:block;
	list-style:none;
	position: relative;
	margin-left: auto;
	margin-right: auto;
}
.selected {
	background: transparent;
}
li:visited, li:active {
	background: transparent;
}
a, a:visited, a:active{
	text-decoration: none;
	background: transparent;
}
.imgframe {
	display: none;
}
.map ul li p{
	display: none;
}
.map ul li label a{
	display: block;
	position:relative;
	color: transparent;
	left:0px;
	top:0px;
	text-align:center;
}
.map input {
	display:none;
}
.map .zone {
	display: block;
	position: absolute;
}
.map .zone:hover{
	border:5px solid gray;
	padding:0px!important;
}
.map li label{
	padding-left: 0px!important;
	display:block!important;
}
.map li {
	background: transparent!important;
	padding:0px;
	text-indent:0px;
}
.map li label a{
	background:transparent!important;
	padding:0px!important;
}
.map li label a{
	margin-top: 10px;
}

"
cd $CFGPATH
for z in *
do
	code=`basename $z`
	. ./$code
	top=`echo $coords | cut -f1 -d, `
	left=`echo $coords | cut -f2 -d, `
	height=`echo $coords | cut -f3 -d, `
	width=`echo $coords | cut -f4 -d, `
	echo ".map #$code {
	top: ${top}px;
	left: ${left}px;
	height: ${height}px!important;	
	width: ${width}px;"
	echo "}"

	echo ".map #$code a {
	display: block;
	height: ${height}px;	
	width: ${width}px;"
	echo "}"
done


