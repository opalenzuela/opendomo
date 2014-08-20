$(function($){
	$("li.plugin").each(function(){
		$(this).attr("style","background-image:url('http://es.opendomo.org/files/" + $(this).attr("id") + ".png')");
	});
	if ($("#code")) {
		$("#managePlugins").attr("style","background:url('http://es.opendomo.org/files/" + $("#code").val() + ".png') no-repeat top left;padding-top:100px;");
	}
});