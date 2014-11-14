setTimeout(checkIfRestarted,5000); // First wait for 5 seconds

function checkIfRestarted() {
	var url = "/data/status.json";
	//$("#reboot_frm").html("<center></center>")
	
	$.post(url)
	.done(function(data) {
		//TODO: check the contents of "data"
		console.log( "success" );
		window.location.reload();
	})
	.fail(function(data) {
		// JSON does not exist yet or connection is not available
		console.log( "failed" );
		setTimeout(checkIfRestarted,1000); // Then check every 2 second 
	});
}