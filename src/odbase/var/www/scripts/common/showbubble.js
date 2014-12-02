function showBubble(bubbleURL, title){
	var f = document.getElementById("bubble_frame");
	if (!f) {
		f= document.createElement("div");
		document.body.appendChild(f);
	}
	f.className ="bubble " + title;
	f.id = "bubble_frame";
	f.innerHTML="<a id='bubble_closebutton' href='javascript:hideBubble()'>Close</a><iframe name='bubble' src='"+bubbleURL+"'></iframe>";
	$("#bubble_frame").show();
}
function hideBubble(){
	$("#bubble_frame").hide();
}