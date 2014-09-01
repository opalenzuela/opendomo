$(function(){
	update_fields();
	var t = document.getElementsByName("mode");
	t[0].onchange = function() {
		update_fields();
	}	
});

function update_fields(){
	var t = document.getElementsByName("mode");
	var ip_li = document.getElementById("ip_li");
	var gw_li = document.getElementById("gw_li");
	var dns_li = document.getElementById("dns_li");
	var mask_li = document.getElementById("mask_li");

	if (t[0].value=='dhcp'){
		ip_li.style.display="none";
		gw_li.style.display="none";
		dns_li.style.display="none";
		mask_li.style.display="none";
	} else {
		ip_li.style.display="";
		gw_li.style.display="";
		dns_li.style.display="";
		mask_li.style.display="";	
	}
}
