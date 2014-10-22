//include_script("/scripts/jquery.longpress.js");

var timetouch = 0;
var focusitem = null;
var milliseconds = 0;

$(function() {
	$("fieldset.selectable li").on("mousedown touchstart", function(event) {
		var url = $(this).find("a").attr("href") ;
		$(this).find("a")
			.attr("href","javascript:void(0)")
			.data("target",url);	
			
		focusitem = $(this);
		timetouch = new Date().getTime();
		//console.log("Pressing on " , $(this));
		event.preventDefault();
		event.stopPropagation();
	});
	
	$("fieldset.selectable li").on("mouseup touchend", function(event) {
		timetouch = 0;
		if (milliseconds<1000 && focusitem != null) {
			var url = $(this).find("a").data("target");
			if (url) {
				console.log("navigating to " + url);
				document.location = url;
			}
		}
		focusitem = null;		
	});	
});

function updatetimers () {
	if (focusitem == null || timetouch == 0) return;
	//console.log("Pressing on ", focusitem);
	var timetouch2 = new Date().getTime();
	var milliseconds = timetouch2 - timetouch;
	if (milliseconds > 1000) {
		console.log("Long pressed on ", focusitem);
		if (focusitem.hasClass("selected")){
			focusitem.removeClass("selected");	
			focusitem.find("input")[0].checked = false;
			console.log("Unselecting")
		} else {
			focusitem.addClass("selected");
			focusitem.find("input")[0].checked = true;
			console.log("Selecting")
		}
		focusitem = null;
		timetouch = 0;
	} else {
		console.log("Short press still");
	}
}
setInterval(updatetimers,100);


