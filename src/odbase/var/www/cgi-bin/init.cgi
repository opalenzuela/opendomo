#!/bin/sh
echo "Content-type: text/html"
echo

show_login_form(){
	echo "<html><head><title>OpenDomo</title>"
	echo "<link rel='stylesheet' type='text/css' href='/cgi-bin/css.cgi?admin'/>";
	echo "</head><body>"
	echo "<div id='header'></div>
<div id='main'>
<form id='loginform_frm' action='/cgi-bin/init.cgi' method='post'>
<fieldset>
 <h1>$1</h1>
 <ul id='login' class='tabform loginform'>
  <li id='USER_lbl' class='input'>
  <label for='USER'>Name:</label>
  <p><input type='text' id='USER' name='USER' value=''/></p></li>
  <li id='PASS_lbl' class='password'>
  <label for='PASS'>Password:</label>
  <p><input type='password' id='PASS' name='PASS'/></p></li>
 </ul>
 <div id='advpad'></div>
 <div class='toolbar' id='login_tbr'>
   <button type='submit'>Restore</button>
 </div>
</fieldset></form>
</div>

<div id='footer'>
<p class='copyright'>
<a href='http://opendomo.org' target='od'>
<img id='logo' src='/images/logo16.png' alt='logo' border='0'> </a> <a href='http://opendomo.com' target='od'>OpenDomo &copy; 2006-2012</a> </p></div>
</body></html>"

}

redirect_to_cgi(){
	echo "<html>
	<head>
		<title>OpenDomo</title>
	</head>
	<body>
		<script type='text/Javascript'>
			window.location='/cgi-bin/od.cgi';
		</script>
		<noscript>
			<frameset rows='*'>
				<frame src='/cgi-bin/od.cgi' scrolling='auto'/>
			</frameset>
			<noframes>
				<a href='/cgi-bin/od.cgi'>Start</a>
			</noframes>
		</noscript>
	</body></html>"
}

redirect_to_wizard(){
	echo "<html>
	<head>
		<title>OpenDomo</title>
	</head>
	<frameset rows='*'>
		<frame src='/wzconf/' scrolling='auto'/>
	</frameset>
	<noframes>
		<a href='/wzconf/'>Start</a>
	</noframes>
	</html>"
}


if test -f /mnt/odconf/sysconf/sysconf.tar.gz
then
	# The system is already configured. Redirect to CGI, as usual
	redirect_to_cgi
else
	# The system is not configured. Is there an encrypted config file?
	if test -f /tmp/cfgbackup.enc
	then
		read variables
		var=`echo $variables | cut -f3 -d=`
		# There is a file, but no parameters. We ask for them:
		if test -z "$var"; then
			show_login_form "Restore OpenDomo configuration"
		else
			# Parameters found! 
			ENC="/tmp/cfgbackup.enc"
			CFGFILE="/mnt/odconf/sysconf/sysconf.tar.gz"
			if openssl enc -aes-256-cbc -salt -in $ENC -out $CFGFILE.tmp -pass pass:$var -d 2>/dev/null
			then
				# Great! Password is correct and configuration restored!
				echo "<html><head><title>Rebooting. Please wait </title>
			<link rel='stylesheet' type='text/css' href='/cgi-bin/css.cgi?admin'/>
			</head><body>
			<div id='header'></div>
			<div id='main'>
			<fieldset>
			 <h1>Rebooting. Please wait</h1>
 			</fieldset>
 			<script type='text/JavaScript'>

function reloadform(){
	window.location='/';
}
window.setTimeout('reloadform()',30000);

 			</script>
 			<div id='footer'></div>
				</body>
				</html>
				"
				mv $CFGFILE.tmp $CFGFILE
				/usr/bin/reboot_u
				echo "REBOOTING"
				echo
			else
				show_login_form "Invalid user/password"
			fi
		fi
	else
		redirect_to_wizard 
	fi
fi
