#!/bin/sh
echo "Content-type: text/html"
echo
THEME=`echo $QUERY_STRING | cut -f1 -d+`
SKIN=`echo $QUERY_STRING | cut -f2 -d+`

if test -z "$THEME"; then 
	THEME="default"
fi

if test -z "$SKIN"; then
	SKIN="silver"
fi

echo "
<html>
 <head>
	<title>OpenDomo</title>
	<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>
	<meta name='viewport' content='width=320; initial-scale=1.0; maximum-scale=1.0; user-scalable=0;' />
	<link rel='shortcut icon' href='/images/logo16.png' type='image/x-icon'/>
	<link rel='stylesheet' type='text/css' href='/themes/$THEME/main.css'/>
	<link rel='stylesheet' type='text/css' href='/skins/$SKIN/main.css'/>
	<script type='text/JavaScript' src='/scripts/common.js'></script>
 </head>
 <body>
<div id='header'>
	<ul id='categ' class='categories'>
		<li id='cat-control' class='tab control'><a href='#'>Control</a></li>
		<li id='cat-config' class='selected config'><a href='#'>Configure</a></li>
		<li id='cat-map' class='tab map'><a href='#'>Map</a></li>
		<li id='cat-tools' class='tab tools'><a href='#'>Tools</a></li>
	</ul>
</div>
  <div id='main'>
<form id='loginform_frm' action='#' method='post'>
<fieldset>
 <h1>Welcome to OpenDomo</h1>
 <ul id='login' class='tabform loginform'>
  <li id='USER_lbl' class='input'><label for='USER'>Username:</label>
	<p><input type='text' id='USER' name='USER' value=""/></p></li>
  <li id='PASS_lbl' class='password'><label for='PASS'>Password:</label>
	<p><input type='password' id='PASS' name='PASS'/></p></li>
 </ul>
 <div id='advpad'></div>
 <div class='toolbar' id='login_tbr'>
  <button>Help</button>
  <button>Enter</button>
 </div>

</div>
</fieldset></form>

   <fieldset>
    <legend>Form example</legend>
    <ul class='tabform'>
	 	<li class='input'><label for='txt'>Text field</label>
			<p><input name='txt' type='text' class='input' value='' /></p></li>     
	 	<li class='list'><label for='lst'>List field</label>
			<p><select name='lst' ><option>first option</option></select></p></li>    
	 	<li class='subcommand'><label for='txt'>Off state</label>
			<p class='DO off'><a href='#' class='sw-on'>on</a><a href='#'  class='sw-off'>off</a></p></li> 
	 	<li class='subcommand'><label for='txt'>On state</label>
			<p class='DO on'><a href='#' class='sw-on'>on</a><a href='#'  class='sw-off'>off</a></p></li> 
		<li id='gaugectl_lbl' class='subcommand[0,10,20,30,40,50,60,70,80,90,100]'><label for='gaugectl'>Gauge Control</label>
			<p class='gauge'><a href='#0'  class='unpressed'>0</a><a href='#'  class='unpressed'>10</a><a href='#20'  class='unpressed'>20</a><a href='#30'  class='unpressed'>30</a><a href='#40'  class='pressed'>40</a><a href='#50'  class='unpressed'>50</a><a href='#60'  class='unpressed'>60</a><a href='#70'  class='unpressed'>70</a><a href='#80'  class='unpressed'>80</a><a href='#90'  class='unpressed'>90</a><a href='#100'  class='unpressed'>100</a></p></li>
		<li id='percent _lbl' class='readonly percent'><label for='percent '>Percentage</label>
			<p><input name='percent ' readonly='true' value='45'/></p></li>


    </ul>
   </fieldset>
 </div>
 </body>
</html>"
echo 
