$(function($){
	$("li.plugin").each(function(){
		$(this).attr("style","background-image:url('http://es.opendomo.org/files/" + $(this).attr("id") + ".png')");
	});
	if ($("#code").val()!=undefined) {
		// only when the plugin details view page is open (not in the main page)
		$("#managePlugins").attr("style","background:url('http://es.opendomo.org/files/" + $("#code").val() + ".png') no-repeat top left;padding-top:128px;");
		$("<iframe width='99%' height='200px' src='" + $("#webp").val() + "'></iframe>").insertAfter("#managePlugins");
	}
});