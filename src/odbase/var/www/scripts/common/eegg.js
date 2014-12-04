function eegg(){
	var m = new Date().getMonth();
	if (m>10 || m<4) {
		$("body").css("overflow","hidden").css("height","100%");
		$("fieldset a").each(function(){
			var s = Math.floor(Math.random()*10);
			var d = Math.floor(Math.random()*10);
			$(this).css("position","absolute")
				.css("top",$(this).position().top +"px")
				.css("left",$(this).position().left +"px");
			$(this).css("transition","all "+s+"s " + d + "s ease-in");
		});	
		$("fieldset a").each(function(){
			var r = Math.floor(Math.random()*360);
			if (m>11||m<2) {
				var col = ["#eee","#efefef","#fefefe"];
			}else {
				var col = ["yellow","orange","red","brown"];
			}
			var c = Math.floor(Math.random()*col.length);
			var l = Math.floor(Math.random()*$("body").width());

			$(this).css("top",$(this).position().top +"px");
			$(this).css("top","750px").css("left",l+"px")
				.css("transform","rotate("+r+"deg)")
				.css("color",col[c]);
		});
	}
}