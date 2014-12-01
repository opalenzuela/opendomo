$(function(){
	$("li.boolean").on("click",function(){
		var c = $(this).find("input").prop("checked");
		if (c!=true) {
			$(this).addClass("checked");
			$(this).removeClass("unchecked");
		} else {
			$(this).addClass("unchecked");
			$(this).removeClass("checked");
		}
	});
	// Refresh all booleans
	$("li.boolean").trigger("click");
	
	$("li.switch, li.pushbutton").each(function(){
		var selID = $(this).find("select").prop("id");
		var s = document.getElementById(selID);
		if (s) {	
			var literal = s.options[s.selectedIndex].value; 
			$(this).addClass("sw_" + literal);
		}
	});
	
	
	$("li.switch").on("click",function(event){
		event.preventDefault();
		var selID = $(this).find("select").prop("id");
		var s = document.getElementById(selID);
		if (s) {
			var literal = s.options[s.selectedIndex].value; 
			$(this).removeClass("sw_" + literal);		
			//if (s.selectedIndex < s.options.length){
			s.selectedIndex++;
			if (s.selectedIndex==-1)
			{
				s.selectedIndex=0;
			}
			literal = s.options[s.selectedIndex].value; 
			$(this).addClass("sw_" + literal);
			//$(this).find("p").text(literal);
		}
	});
	
	$("li.pushbutton").on("touchstart mousedown",function(event){
		event.preventDefault();
		var t = $(this);
		t.removeClass("sw_off");
		if (!t.hasClass("sw_on")) {
			t.addClass("sw_on");
			t.find("select").val("on");
		}
	});
	
	$("li.pushbutton").on("touchend mouseup",function(event){
		event.preventDefault();
		var t = $(this);
		t.removeClass("sw_on");
		if (!t.hasClass("sw_off")) {
			t.addClass("sw_off");
			t.find("select").val("off");
		}
	});	
});