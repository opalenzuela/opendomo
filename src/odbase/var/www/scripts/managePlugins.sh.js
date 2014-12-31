$(function($){
	$("li.plugin").each(function(){
		$(this).attr("style","background-image:url('/data/" + $(this).attr("id") + ".png')");
	});
	if ($("#code").val()!=undefined) {
		// only when the plugin details view page is open (not in the main page)
		$("#managePlugins").attr("style","background:url('/data/" + $("#code").val() + ".png') no-repeat top left;padding-top:100px;background-size:100px;");
		//$("<iframe width='99%' height='200px' src='" + $("#webp").val() + "'></iframe>").insertAfter("#managePlugins");
	}
});

setInterval(updatePluginStates,5000);

function updatePluginStates (){
	$.get("/cgi-bin/od.cgi/managePlugins.sh?GUI=XML",function(response){
		jQuery(response).find("item").each(function(index){
			try 
			{
				var myid = this.getAttribute("name");
				var c = this.getAttribute("class");
				
				$("#" + myid).removeClass("new")
					.removeClass("installed")
					.removeClass("inprogress")
					.removeClass("failed")
					.addClass(c);
			}catch(e) {
				console.log("Exception: " + e.message);
			}
		});
	});
}