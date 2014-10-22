$(function(){
	navigator.geolocation.getCurrentPosition(function(pos){
		var lat = pos.coords.latitude;
		var lon = pos.coords.longitude
		var tim = new Date().getTime()/1000;
		console.log("here I am:" + lat + " " + lon)
		var url = "https://maps.googleapis.com/maps/api/timezone/json?location=" + lat + "," + lon + "&timestamp=" + tim + "&sensor=false";
		var timezone = loadJSON(url);
		try {
			$("#latitude").val(lat);
			$("#longitude").val(lon);
			$("#timezoneid").val(timezone.timeZoneId);
			$("#timezone").val(timezone.rawOffset / 3600);
			
			console.log("City: " + timezone.timeZoneId);
			console.log("TZC: " + timezone.rawOffset / 3600);
			console.log("Timezone name: " + timezone.timeZoneName);
			var url = "https://maps.googleapis.com/maps/api/geocode/json?latlng=" + lat + "," + lon + "&sensor=false";
		} catch (e) {
			console.log("Error: "+e.message);
		}
		var location = loadJSON(url);
		try {
			console.log ("Status: " + location.status);
			if (location.status=="OK") {
				console.log(location.results[0]);
				var c = location.results[0].address_components;
				for (var i=0;i<c.length;i++){
					if (c[i].types[0]=="locality"||c[i].types[1]=="locality") {
						$("#city").val(c[i].long_name);
					}
				} 
				$("#address").val(location.results[0].formatted_address);
			}
		} catch(e) {
			console.log("Error 2: "+e.message);
		}
	});

});
