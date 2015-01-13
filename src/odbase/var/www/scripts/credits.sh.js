$(function(){
	$("li.user").each(function(){
		var url = "https://github.com/" + $(this).prop("id");
		$(this).find("a").prop("target","new").prop("href",url);
	});
});