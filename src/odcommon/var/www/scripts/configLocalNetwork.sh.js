function init_form(){
	update_fields();
	var t = document.getElementsByName("mode");
	t[0].onchange = function() {
		update_fields();
	}	
}

function update_fields(){
	var t = document.getElementsByName("mode");
	var ip_li = document.getElementById("ip_li");
	var gw_li = document.getElementById("gw_li");
	var ntp_li = document.getElementById("ntp_li");
	var dns_li = document.getElementById("dns_li");

	if (t[0].value=='dhcp'){
		ip_li.style.display="none";
		gw_li.style.display="none";
		ntp_li.style.display="none";
		dns_li.style.display="none";
	} else {
		ip_li.style.display="";
		gw_li.style.display="";
		ntp_li.style.display="";
		dns_li.style.display="";	
	}
}
