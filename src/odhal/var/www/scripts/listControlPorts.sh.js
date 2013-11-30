


$(document).ready(function() 
{
	/*actualiza los puertos de entrada*/
	$(function() 
	{
  		setInterval(updatePorts, 2000);
	});
	
	function updatePorts()
	{
$(document).ready(function(){
	$.ajax({
		type: "GET",
		url: "/cgi-bin/od.cgi/control/listControlPorts.sh?GUI=XML",
		dataType: "xml",
		success: function(xml) {
			$(xml).find('text').each(function(){ // puertos de entrada
				var id = $(this).attr('id');
				if(id.indexOf("WAR") == -1 && id.indexOf("INF") == -1 && id.indexOf("ERR") == -1)
				{
				obj = document.getElementById(id);
				var status = $(this).text();

			  	 $(obj).attr('class', status);
				 $(obj).text(status);
				}


			});
			
			$(xml).find('commands').each(function(){ // puertos de salida
				var status = "";
				$(this).find('command').each(function(){
					if(status == "")
						status = $(this).text();
				});
				var id = $(this).attr('id');
				obj = document.getElementById(id);


			  	 $(obj).attr('class', "DO "+status);


			});
		}
	});
});
	

	}	

});
