#!/bin/sh
#desc:Test interface
#package:odcgi
#type:local

# This script contains all the possible blocks used in the CGI.
# Before approving a new theme or skin, it should be verified that the total
# display is acceptable.

S=`basename $0`

echo "#INFO Parameters received: $1 $2 $3"

echo "#>Normal passive list"
echo "list:$S"
echo "	item1	Item 1	item"
echo "	item2	Item 2	item"
echo

echo "#>Normal active list, with actions"
echo "list:$S"
echo "	-item1	Item 1	item"
echo "	-item2	Long description item 2	item"
echo "actions:"
echo "	$S	Refresh"
echo

echo "#>Detailed list"
echo "list:$S	detailed"
echo "	item1	Item 1	item	The detail"
echo "	item2	Long description item 2	item	The detail"
echo

echo "#>Icon list"
echo "list:$S	iconlist"
echo "	item1	Item 1	item"
echo "	item2	Item 2	item"
echo "	item3	Item 3	file"
echo "	item4	Item 4	dir"
echo "actions:"
echo "	goback	Back"
echo

echo "#>Data form"
echo "form:$S"
echo "	none1	Separator1	separator"
echo "	passw	Password	password"
echo "	reado	Readonly field	readonly	Field value"
echo "	item1	Text field	text	My text"
echo "	item2	E-mail address	email	myemail@opendomo.com"
echo "	item3	Date field	date"
echo "	none2	Lists	separator"
echo "	item4	Anon. list	list[a,b,c,d,e]	c"
echo "	item5	Named list	list[a:A,b:B,c:C]	b"
echo "	none3	Controls	separator"
echo "	subc1	Subcommand	subcommand[on,off]	on"
echo "	subc2	Subcommand 2	subcommand[00,10,20,30]	20"
echo "	none4	Media	separator"
echo "	med01	Image	image	/images/nocam.png"
echo "	med02	Video	video	/images/nocam.png"
echo "	med03	Application	application	/es.license.html"
echo "actions:"
echo "	$S	Send"
echo


echo "#>Data form wizard"
echo "form:$S	wizard"
echo "	none1	Separator1	separator"
echo "	passw	Password	password"
echo "	reado	Readonly field	readonly	Field value"
echo "	item1	Text field	text	My text"
echo "	item2	E-mail address	email	myemail@opendomo.com"
echo "	item3	Date field	date"
echo "	none2	Lists	separator"
echo "	item4	Anon. list	list[a,b,c,d,e]	c"
echo "	item5	Named list	list[a:A,b:B,c:C]	b"
echo "	none3	Controls	separator"
echo "	subc1	Subcommand	subcommand[on,off]	on"
echo "	subc2	Subcommand 2	subcommand[00,10,20,30]	20"
echo "	none4	Media	separator"
echo "	med01	Image	image	/images/nocam.png"
echo "	med02	Video	video	/images/nocam.png"
echo "	med03	Application	application	/es.license.html"
echo "actions:"
echo "	$S	Send"
echo

echo "#>Data form with explanation"
echo "form:$S"
echo "	:	The first explanation	:"
echo "	:	Second explanation	:"
echo "	none1	Lists	separator"
echo "	item5	Text field	text	b"
echo "actions:"
echo "	$S	Send"
echo


echo "#>Line graph"
echo "graph:line$S"
echo "	dataset1	Dataset 1	[[0,2],[1,3],[2,1],[3,4]]"
echo "	dataset2	Dataset 2	[[0,6],[1,2],[2,3],[3,1]]"
echo

echo "#>Bars graph, with actions"
echo "graph:bar$S	bars"
echo "	dataset1	Dataset 1	[[2,0],[3,1],[1,2],[4,3]]"
echo "	dataset2	Dataset 2	[[6,0],[2,1],[3,2],[1,3]]"
echo "actions:"
echo "	goback	Back"
echo

echo "#>Pie graph"
echo "graph:pie$S	pie"
echo "	dataset1	Dataset 1	30"
echo "	dataset2	Dataset 2	15"
echo

echo "# Regular comment"
echo "#INFO Informative message"
echo "#WARN Warning message"
echo "#ERR Error message"
echo "#URL http://www.opendomo.org"
echo

echo "#>Wizard list with explanation	wizard"
echo "list:$S	wizard"
echo "	:	This is the explanation	:"
echo "	item1	Item 1	item"
echo "	item2	Item 2	item"
echo "actions:"
echo "	goback	Back"
echo

echo "#>Wizard form with explanation	wizard"
echo "form:$S	wizard"
echo "	:	This is the explanation	:"
echo "	item1	Item 1	text"
echo "	item2	Item 2	list[on,off]"
echo "actions:"
echo "	goback	Back"
echo
