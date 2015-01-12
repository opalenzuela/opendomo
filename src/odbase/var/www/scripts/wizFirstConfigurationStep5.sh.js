var interval = setInterval(function () {
	$("button[type=submit]").hide();
	$("p.info").hide();	
	try {
		var prompting = loadTXT("/data/status.json");
		if (prompting.indexOf("started")>0 || prompting.indexOf("active")>0 ) {
			$("button[type=submit]").show();
			$("p.warning").hide();
			$("p.info").show();
			clearInterval(interval);
		}
	} catch (e) {
		// Silently quit
	}
}, 1000);