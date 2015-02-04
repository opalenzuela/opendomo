setTimeout(checkIfRestarted,5000); // First wait for 5 seconds

function checkIfRestarted() {
	var url = "/data/status.json";
	//$("#reboot_frm").html("<center></center>")
	
	$.post(url)
	.done(function(data) {
		try {
			if ((typeof data == "object") && (data.status == "active")>-1) {
				console.log( "success" );
				window.location.replace("/cgi-bin/od.cgi/control/");
			} else {
				console.log( "not ready yet: " + data );
				setTimeout(checkIfRestarted,1000); // Then check every second 			
			}
		} catch (e) {
			setTimeout(checkIfRestarted,1000); // Check again later
		}
	})
	.fail(function(data) {
		// JSON does not exist yet or connection is not available
		console.log( "failed" );
		setTimeout(checkIfRestarted,1000); // Then check every second 
	});
}