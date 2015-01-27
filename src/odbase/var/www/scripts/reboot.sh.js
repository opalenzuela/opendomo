setTimeout(checkIfRestarted,5000); // First wait for 5 seconds

function checkIfRestarted() {
	var url = "/data/status.json";
	//$("#reboot_frm").html("<center></center>")
	
	$.post(url)
	.done(function(data) {
		//TODO: check the contents of "data"
		if (data.indexOf("active")>-1) {
			console.log( "success" );
			window.location.replace("/cgi-bin/od.cgi/control/");
		} else {
			console.log( "not ready yet: " + data );
			setTimeout(checkIfRestarted,1000); // Then check every second 			
		}
	})
	.fail(function(data) {
		// JSON does not exist yet or connection is not available
		console.log( "failed" );
		setTimeout(checkIfRestarted,1000); // Then check every second 
	});
}