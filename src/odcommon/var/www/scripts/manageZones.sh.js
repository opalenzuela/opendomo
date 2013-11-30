setInterval(function(){
	var manageZones_frm = document.getElementById("manageZones_frm");
	var addZone_frm = document.getElementById("addZone_frm");
	if (manageZones_frm) {
		manageZones_frm.coord.onchange=updateMap();
	}
	if (addZone_frm) {
		addZone_frm.coord.onchange=updateMap();
	}
},1000);

function updateMap(){
/*	var manageZones_frm = document.getElementById("manageZones_frm");
	var addZone_frm = document.getElementById("addZone_frm");
	if (manageZones_frm) {
		var c = manageZones_frm.coord.value;
	} else {
		var c = addZone_frm.coord.value;
	} */
	var f = $("input[name=coord]");
	var c = f.val();
	if (c) {
		var a = c.split(",");
		var o = document.getElementById("demozone");
	
		if (o) {
			o.style.top=a[0]+"px";
			o.style.left=a[1]+"px";
			o.style.height=a[2]+"px";
			o.style.width=a[3]+"px";
		}
	}
}
