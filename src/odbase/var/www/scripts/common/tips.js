$(function(){
	try {
		if (typeof localStorage.tips == "undefined") {
			localStorage["tip:"]=[];
		}
		if ($("p.tip").length > 0) {
			$("body").append("<div id='floattip'></div>");
			$("#floattip").on("click", function(){
				$("p.tip").show();
				$(this).hide();
			});
		}
		$("p.tip").each(function(){
			var t = $(this).text();
			if (localStorage["tip:"+t] == undefined) {
				$(this).show();
			} else {
				$(this).hide();
			}
		});
		$("p.tip").on("click touch",function(){
			var t = $(this).text();
			localStorage["tip:"+t] = "checked";
			$(this).hide();
			$("#floattip").show();
		});
	} catch(e){
	}
});