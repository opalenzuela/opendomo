$(function(){
	$("button").prop("disabled",true).css("opacity","0.3");
	$("#npassw_lbl").append("<i></i>");
	$("#passw_lbl").append("<i></i>");
	$("#npassw, #passw, #opassw").on("change", function(){
		var prev = $("#opassw").val();
		if (prev.length>=6) {
			$("#opassw").css("border-color","");
		}else {
			$("#opassw").css("border-color","red");
		}
			
		var np1 = $("#npassw").val();
		var np2 = $("#passw").val();
		if ( (np1 == np2) && (np1.length>=6) ) {
			$("button").prop("disabled",false).css("opacity","1");
			$("#npassw, #passw").css("border-color","");
		} else {
			$("#npassw, #passw").css("border-color","red");
		}
		
		$("#npassw_lbl>i").text("   (" + np1.length + "/6)");
		$("#passw_lbl>i").text("   (" + np2.length + "/6)");
	});
});
