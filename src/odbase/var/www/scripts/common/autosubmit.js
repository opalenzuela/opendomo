$(function(){
	$("fieldset.autosubmit input, fieldset.autosubmit select").on("change",function(){
		var id = $(this).closest("form").attr("id");
		submitForm(id);
	});
	// Submit button is not necessary anymore:
	$("fieldset.autosubmit button[type=submit]").hide();
});

function submitForm(formID) {
	var dataString = $("#"+formID).serialize();
	var url = $("#"+formID).attr("action")+"?GUI=XML";
 
	$.post(url,dataString)
	.done(function(data) {
		//TODO: Update field values from response
		console.log( "success" );
		console.log(data);
	})
	.fail(function(data) {
		//TODO: Show some feedback in case of failure
		console.log( "failed" );
		console.log(data);
	});
}