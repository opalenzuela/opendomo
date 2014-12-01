$(function(){
	if(("standalone" in window.navigator) && window.navigator.standalone){
		jQuery("a").on("click",function(e){
			console.log("Clicked",e);
			var href = $(this).attr("href");
			if (href) document.location.href = href
			e.preventDefault();
		});
	} 
});
