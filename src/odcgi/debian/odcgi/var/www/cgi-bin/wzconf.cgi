#!/bin/sh
echo "Content-type: text/html"
echo

echo "<html>
  <head>
	<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>
	<title>OpenDomo $HELP</title>
	<!-- <link rel='stylesheet' type='text/css' href='/cgi-bin/css.cgi'/> -->
	<link rel='stylesheet' type='text/css' media='screen' href='/wzconf/wzconf_styles.css' />
  </head>
  <body>
	<a name='top'></a>
	<div class='toolbar'>
		<a class='homebtn' href='/cgi-bin/wzconf.cgi'><img src='/images/index.gif'></a> 
		<div style='float:right'>
			<a class='closebtn' href='/cgi-bin/od.cgi/control/' target='_top'>
				<img src='/images/close_small.gif'></a>	
		</dev>
	</div>
	<h1>Manuals</h1>
	<ul id='toc' class='listbox'>
"
cd /var/www
grep h1 wzconf/*.html  |\
	 sed -e 's/^\([^:]*\):*/<li class=wiz><a href="\/\1" >/' -e 's/$/<\/a><\/li>/' -e 's/h1/label/g'

echo "
	</ul>
</html>"

