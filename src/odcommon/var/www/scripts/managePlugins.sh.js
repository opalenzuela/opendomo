$(function($){
	$("li.plugin").each(function(){
		$(this).attr("style","background-image:url('http://es.opendomo.org/files/" + $(this).attr("id") + ".png')");
	});
});