$("select[name=model]").on("change",function(){
	switch(jQuery(this).val()){
		case "dummy":
			$("#port_li").hide();
			$("#dirname_li").show();
			$("#refresh_li").hide();
			$("#ip_li").hide();
			$("#username_li").hide();
			$("#password_li").hide();
			break;
		case "micropik":
			$("#port_li").show();
			$("#dirname_li").show();
			$("#refresh_li").hide();
			$("#ip_li").hide();
			$("#username_li").hide();
			$("#password_li").hide();
			break;
		case "arduino":
			$("#port_li").show();
			$("#dirname_li").show();
			$("#refresh_li").hide();
			$("#ip_li").hide();
			$("#username_li").hide();
			$("#password_li").hide();
			break;	
		case "x10":
			$("#port_li").show();
			$("#dirname_li").show();
			$("#refresh_li").hide();
			$("#ip_li").hide();
			$("#username_li").hide();
			$("#password_li").hide();
			break;
		case "domino":
			$("#port_li").show();
			$("#dirname_li").show();
			$("#refresh_li").hide();
			$("#ip_li").show();
			$("#username_li").hide();
			$("#password_li").hide();
			break;
		} 
});