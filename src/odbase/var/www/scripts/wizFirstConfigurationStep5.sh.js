$(function(){
	$("button[type=submit]").hide();
});

setInterval(function () {
	try {
		var prompting = loadTXT("/data/status.json");
		if (prompting.indexOf("started")>0 || prompting.indexOf("active")>0 ) {
			$("button[type=submit]").show();
			$("p.info").hide();
		}
	} catch (e) {
		// Silently quit
	}
}, 1000);