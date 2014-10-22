function init_form(){
	var im = $("#imgframe_image");
	var theImage = new Image();
	theImage.src = im.attr("src");	
	$("input[name='imagw']").val(theImage.width);
	$("input[name='imagh']").val(theImage.height);
}
