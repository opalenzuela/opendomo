document.getElementsByClassName = function(cl) {
	var retnode = [];
	var myclass = new RegExp('\\b'+cl+'\\b');
	var elem = this.getElementsByTagName('*');
	for (var i = 0; i < elem.length; i++) {
		var classes = elem[i].className;
		if (myclass.test(classes)) retnode.push(elem[i]);
	}
	return retnode;
}; 

function addToolbarButton(tbname, text, action, classes){
	var btn = document.createElement('button');
	var tbr = document.getElementById(tbname+'_tbr');
	if (tbr) {
		btn.innerHTML=text;
		btn.type='button';
		btn.className=classes;
		btn.onclick= new Function(action);
		tbr.appendChild(btn);
	} else {
		//alert("Toolbar " + tbname + " not found!");
	}
} 

function include_script(file) {
   if (document.createElement && document.getElementsByTagName) {
     var head = document.getElementsByTagName('head')[0];

     var script = document.createElement('script');
     script.setAttribute('type', 'text/javascript');
     script.setAttribute('src', file);

     head.appendChild(script);
   }
}

// This function can link two dropdowns, so the second one's data depends on
// the item selected on the first one
function linkDropdowns(s,d){
	s.onchange = function(){
		for (i=0;i<d.children.length;i++){
			if (d.children[i].id == s.value) {
				show_optiongroup(d.children[i]);
				if (d.children[i].children[0]) {
					d.children[i].children[0].selected = true;
				}
			} else {
				hide_optiongroup(d.children[i]);
			}
			d.children[i].label="";
		}
		if (d.onchange) d.onchange();
	}
	s.onchange();
}
// Similar to linkDropdown, this function does basically the same, but
// attaching the event to another dropdown
function crossLinkDropdowns(e,s,d){
	e.onchange = function(){
		for (i=0;i<d.children.length;i++){
			if (d.children[i].id == s.value) {
				show_optiongroup(d.children[i]);
				if (d.children[i].children[0]) {
					d.children[i].children[0].selected = true;
				}
			} else {
				hide_optiongroup(d.children[i]);
			}
			d.children[i].label="";
		}
		if (d.onchange) d.onchange();
	}
}

function hide_optiongroup(o){
	var is_chrome = navigator.userAgent.toLowerCase().indexOf('chrome') > -1;
	o.label="";
	if (is_chrome){
		// Nasty browser that does not support hiding optgroups
		o.disabled=true;
	} else {
		o.hidden=true;
	}
}

function show_optiongroup(o){
	var is_chrome = navigator.userAgent.toLowerCase().indexOf('chrome') > -1;
	o.label="";
	if (is_chrome){
		o.disabled=false;
	} else {
		o.hidden=false;
	}
	//o.innerHTML = 
}

function loadDropdownFromArray(dropdown,arraydata,arraydata_name,section){
	// First group is for system variables
	if (section!=""){
		var vgrp= document.createElement("optgroup");
		vgrp.label=section;
		dropdown.appendChild(vgrp);
	}

	for (i=0;i<arraydata.length; i++){                                      
		dropdown.options[dropdown.options.length] = new Option(arraydata_name[i],arraydata[i]);
	}
}

function getSelectedElementsFromList(lstname){
	var value = "";
	var lsb = document.getElementById(lstname+'_frm');
	if (lsb) {
		for (i=0;i<fields.length;i++) {
			if ((fields[i].type == "checkbox") && fields[i].checked == true){
				value = value + ',' + fields[i].value;
			}
		}
	} else {
		//alert("List " + lstname + " not found!");
	}
	return value;
}

function getLastSelectedElementFromList(lstname){
	var value = "";
	var lsb = document.getElementById(lstname+'_frm');
	if (lsb) {
		for (i=0;i<fields.length;i++) {
			if ((fields[i].type == "checkbox") && fields[i].checked == true){
				value = fields[i].value;
			}
		}
	} else {
		//alert("List " + lstname + " not found!");
	}
	return value;
}

function putFlags(){
	untrans = document.getElementsByClassName("untrans");
	for (i=0;i<untrans.length;i++) {
		var transbutton = document.createElement("a");
		transbutton.innerHTML="<img src='/images/trans.png' alt='translate'/>";
		transbutton.onmousedown=function() {
			translateMe(this.parentNode);
			return false;
		}
		untrans[i].appendChild(transbutton);
	}
}

function tryToReloadCam(image){
	image.src="/images/nocam.png";
}

function showTT(id){
	var tt = document.getElementById(id+'_tt');
	var lbl = document.getElementById(id+'_lbl');
	var curleft = 20;
	var curtop = 0;
	
	if (lbl && lbl.offsetParent) {
		do {
			curleft += lbl.offsetLeft;
			curtop += lbl.offsetTop;
		} while (lbl = lbl.offsetParent);
	}
	
	if (tt){
		tt.left = curleft;
		tt.top = curtop;
		tt.style.display='block'; 
	}	
}
function hideTT(id){
	var tt = document.getElementById(id+'_tt');
	if (tt) tt.style.display='none'; 
}

function notifyProblem(){
	openPopup('http://www.opendomo.com/wiki/index.php?action=edit&title=Discusión:'+basename(location.pathname));
}

function help(topic){
	window.open('http://www.opendomo.com/wiki/index.php?title='+topic,'help');
}


function addJavascript(filename){
	  var fileref=document.createElement('script')
	  fileref.setAttribute("src", "/scripts/" + filename)
	  fileref.setAttribute("type","text/javascript")
}

function initialise(){
	addJavascript("jquery.min.js");
	/* Experimental assistive keyboard
	ctrls = document.getElementsByTagName("INPUT");
	for (i=0;i<ctrls.length;i++){
		if (ctrls[i].type=="text" || ctrls[i].type=="password"){
			helphtml = "<img src='' onclick=\"shKeyboard('"+ctrls[i].name+"');\"/>"
			pn = ctrls[i].parentNode;
			pn.innerHTML = pn.innerHTML + helphtml;
		}
	}
	*/
	init_focus();

	
	/* BOTONES ON OFF */
	$('a:regex(id,^cmd_.*_(on|off)$)').click(function(e)
	{
			var $me = $(this);
	    		var $my_parent = $me.parent();

			e.preventDefault();
			href = e.target.href;
			$.ajax({
			  url: href,
	  			success: function()
			{
				if(e.currentTarget.childNodes[0].textContent == 'on')
			  	 $my_parent.attr('class', 'DO on');
				else
			  	 $my_parent.attr('class', 'DO off');
			  }
			});
		
	});

	/*
	imgs = document.getElementsByTagName("img");
	for (i=0;i<imgs.length;i++) {
		adjust_img(imgs[i]);
	}
	*/
	/*
	lists = document.getElementsByTagName("ul");
	for (i=0;i<lists.length;i++){
		if(hasClassName(lists[i],"listbox")){
			add_listbox_toolbar(lists[i]);
		}
	}
	*/

	cvs = document.getElementsByTagName("canvas");
	if (cvs.length>0) init_graph_lib();
	for (i=0;i<cvs.length;i++){
	
	}

	login = document.getElementById("USER");
	if (login) {
		login.focus();
	}	
	if (window.init_form) init_form();
}

function init_graph_lib(){
	addJavascript("jquery.flot.min.js");
}

function translateMe(tag){
	var text=tag.firstChild.data;
	var id=tag.id;
	var lang=tag.lang;
	var url="http://cloud.opendomo.com/babel/trans.php";
	var script="";
	while (tag.parentNode.tagName!="BODY" && tag.parentNode) {
		tag = tag.parentNode;
		if (tag.tagName=="UL") script = tag.id + ".sh";
	}
	openPopup(url+"?data="+text+"&id="+id+"&lang="+lang+"&script="+script);
}

function advscreen(page){
    tmp = new ajaxObject ('advpad', page);
    tmp.update();
}

// Abbrev. of "Input Key", for the numeric keyboard
function ik(key){
	var passfield = document.getElementById("PASS");
	passfield.value = passfield.value + key;
}

var iconlist='iconlist';
var detailedlist='detailedlist';
var listbox='listbox';
function add_listbox_toolbar(list){
	var tbar = document.createElement('div');
	tbar.setAttribute('align','right');
	tbar.className='listboxtoolbar';
	tbar.innerHTML="<img src='/images/iconlist.png' onclick='sel_view_mode(this,iconlist)'/><img src='/images/detailedlist.png' onclick='sel_view_mode(this,detailedlist)'/><img src='/images/listbox.png' onclick='sel_view_mode(this,listbox)'/>";
	
	//TODO Insert the toolbar in the "legend" box when the ID field is corrected
	//list.parentNode.childNodes[1].appendChild(tbar);
	list.parentNode.insertBefore(tbar,list);
}

function sel_view_mode(list,mode){
//	alert(list.parentNode.parentNode.childNodes[4]+'.className= '+mode);
	list.parentNode.parentNode.childNodes[4].className=mode;
}

function init_focus(){
	if (document.forms.length>0) {
		fields = document.forms[0].elements;
		for(i = 0; i < fields.length; i++) {
			if(fields[i].type == 'radio' || fields[i].type == 'checkbox' || fields[i].type == 'file') {
				fields[i].onclick = function(){addClassName(this.parentNode.parentNode, "focused", true)};
				fields[i].onfocus = function(){addClassName(this.parentNode.parentNode, "focused", true)};
			}
			else if (fields[i].type == 'text' || fields[i].type == 'password' || fields[i].type == 'select-one') {
				fields[i].onfocus = function(){addClassName(this.parentNode.parentNode, "focused", true)};
				fields[i].onblur = function(){removeClassName(this.parentNode.parentNode, "focused")};
			}
			else if (fields[i].type == 'button' || fields[i].type == 'submit' || fields[i].type == 'reset') {
				// nothing
			}
			else {
				//if (fields[i].type) alert(fields[i].type);
			}
		}
	}
}

//http://www.bigbold.com/snippets/posts/show/2630
function addClassName(objElement, strClass, blnMayAlreadyExist){
   if ( objElement.className ){
      var arrList = objElement.className.split(' ');
      if ( blnMayAlreadyExist ){
         var strClassUpper = strClass.toUpperCase();
         for ( var i = 0; i < arrList.length; i++ ){
            if ( arrList[i].toUpperCase() == strClassUpper ){
               arrList.splice(i, 1);
               i--;
             }
           }
      }
      arrList[arrList.length] = strClass;
      objElement.className = arrList.join(' ');
   }
   else{  
      objElement.className = strClass;
      }
}

//http://www.bigbold.com/snippets/posts/show/2630
function removeClassName(objElement, strClass){
   if ( objElement.className ){
      var arrList = objElement.className.split(' ');
      var strClassUpper = strClass.toUpperCase();
      for ( var i = 0; i < arrList.length; i++ ){
         if ( arrList[i].toUpperCase() == strClassUpper ){
            arrList.splice(i, 1);
            i--;
         }
      }
      objElement.className = arrList.join(' ');
   }
}

function hasClassName(objElement, strClass){
   if ( objElement.className ){
      var arrList = objElement.className.split(' ');
      var strClassUpper = strClass.toUpperCase();
      for ( var i = 0; i < arrList.length; i++ ){
         if ( arrList[i].toUpperCase() == strClassUpper ){
            return true;
         }
      }
      objElement.className = arrList.join(' ');
   }
   return false;
}


// Ajusta las imágenes a su contenedor
/*function adjust_img(obj){
        var ph = obj.parentNode.offsetHeight-10;
        var pw = obj.parentNode.offsetWidth-10;
        var H = obj.height;
        var W = obj.width;

        // Si la imagen no cabe en el marco, ajustar
        if ((ph <= H) || (pw <= W)){
//                document.title=("Image does not fit!"+H+"x"+W+" > "+ph+"x"+pw);
                if(ph/H < pw/W){
                        var R = ph/H;
                }else{
                        var R = pw/W;
                }
                obj.height = Math.round(H * R);
                obj.width = Math.round(W * R);
        }
        //obj.style.display="block";
}*/



// Integración no instrusiva del evento onload
function addEvent(obj, evType, fn){
 if (obj.addEventListener){
   obj.addEventListener(evType, fn, false);
   return true;
 } else if (obj.attachEvent){
   var r = obj.attachEvent("on"+evType, fn);
   return r;
 } else {
   return false;
 }
}
addEvent(window, 'load', initialise);

// Esta rutina deberá crear un teclado en pantalla para dispositivos táctiles
function shKeyboard(fieldname){

}

function isValid(ctrl,regexp){
	var re = new RegExp(regexp);
	var frm = ctrl.form;
	if (ctrl.value.match(re)) {
		removeClassName(ctrl,"errfield");
		for (i=0; i< frm.elements.length;i++){
			if (frm.elements[i].type=='submit'){
				frm.elements[i].disabled=false;	
			}
		}
	} else {
		addClassName(ctrl, "errfield", true);
		for (i=0; i< frm.elements.length;i++){
			if (frm.elements[i].type=='submit'){
				frm.elements[i].disabled=true;	
			}
		}
	}

}

function openPopup(url){
  window.open(url,'opendomo','toolbar=no,menubar=no,statusbar=no,scrollbars=1,width=632,height=416');
}

function showDebug(){
	var dbg = document.getElementById('debug_box');
	if (dbg.style.display == 'block') {
		dbg.style.display='none';
	} else {
		dbg.style.display='block';
	}
}

function openSSH(){
	window.location = "ssh://" + window.location.host;
}

function basename(path) {
    return path.replace(/\\/g,'/').replace( /.*\//, '' );
}

/* JQUERY DEPENDENT */
jQuery.expr[':'].regex = function(elem, index, match) {
    var matchParams = match[3].split(','),
        validLabels = /^(data|css):/,
        attr = {
            method: matchParams[0].match(validLabels) ? 
                        matchParams[0].split(':')[0] : 'attr',
            property: matchParams.shift().replace(validLabels,'')
        },
        regexFlags = 'ig',
        regex = new RegExp(matchParams.join('').replace(/^\s+|\s+$/g,''), regexFlags);
    return regex.test(jQuery(elem)[attr.method](attr.property));
} 

/* Funciones para las graficas*/
function showTooltip(x, y, contents) 
{
        $('<div id="tooltip">' + contents + '</div>').css( {
            position: 'absolute',
            display: 'none',
            top: y + 5,
            left: x + 5,
            border: '1px solid #cccccc',
            padding: '2px',
            'background-color': '#dddddd',
            opacity: 0.80
        }).appendTo("body").fadeIn(200);
}

function timeConverter(UNIX_timestamp, type)
{
	var a = new Date(UNIX_timestamp*1000);
	var months = ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];

	timeOffsetInHours = (new Date().getTimezoneOffset()/60) * (-1);

	a.setHours(a.getHours() - timeOffsetInHours);


     if(type == "date")
     {
	   var year = a.getFullYear();
	   var month = months[a.getMonth()];
	   var date = a.getDate();
	   var time = date+'/'+month+'/'+year;
     }	
     else if(type == "month")
     {
	   var year = a.getFullYear();
	   var month = months[a.getMonth()];
	   var time = month+'/'+year;
     }	
     else if(type == "hour")
     {
	   var year = a.getFullYear();
	   var month = months[a.getMonth()];
	   var date = a.getDate();
	   var hour = a.getHours();
     	   var min = a.getMinutes();

	   var time = date+'/'+month+'/'+year + ' ' + hour+':00h';

     }
     else if(type == "time")
     {
	   var hour = a.getHours();
     	   var min = a.getMinutes();
	   var sec = a.getSeconds();
	   var time = hour+':'+min ;
     }
     return time;
}

previousPoint = null;            
function prepareTooltip(event, pos, item) 
{
    $("#x").text(pos.x.toFixed(2));
    $("#y").text(pos.y.toFixed(2));


         if (item) 
         {

             if (previousPoint != item.dataIndex) {
                  previousPoint = item.dataIndex;
                   
                  $("#tooltip").remove();
                  var x = item.datapoint[0].toFixed(2),
                   y = item.datapoint[1].toFixed(2);

		var timeOffset = 0;
		
		if(item.seriesIndex == 1)
		{
			timeOffset = $('input[name="offset"]').val();  
		}
		

	        if(item.series.xaxis.options.minTickSize != null)
		{
	        	  if(item.series.xaxis.options.minTickSize[1] == "day")
			  {
				x = timeConverter((x/1000)-timeOffset,"date");
			  }
			  else if(item.series.xaxis.options.minTickSize[1] == "month")
			  {
				x = timeConverter((x/1000)-timeOffset,"month");
			  }
			  else if(item.series.xaxis.options.minTickSize[1] == "hour")
			  {
				x = timeConverter((x/1000)-timeOffset,"hour");
			  }
			  else if(item.series.xaxis.options.minTickSize[1] == "minute")
			  {
				x = timeConverter((x/1000)-timeOffset,"time");
			  }
		}
		
		var active_unit = $('input[name="active_unit"]').val();
		

/*
                  showTooltip(item.pageX, item.pageY,
                              item.series.label + " of " + x + " = " + y);
*/
                  showTooltip(item.pageX, item.pageY, x + " = " + y + " "+ active_unit );
              }
            }
            else {
                $("#tooltip").remove();
                previousPoint = null;            
            }
}
/* END Funciones para las graficas*/


/*error notifications
var ntotalnotif = 0;
var notifshowed = 0;
function shower(class_name)
{

	$(class_name).each(function(index) 
	{
	notifshowed++;
	if(index == 0)
	{
 	 $(this).attr('display', '');
	 $(this).fadeIn(400);
	}
	else if(index == 1)
	{
 	 $(this).delay(4000);
 	 $(this).attr('display', '');
	 $(this).delay(1000).fadeIn(400);
	}
	else if(index == 2)
	{
 	 $(this).delay(4000);
 	 $(this).attr('display', '');
	 $(this).delay(1000).fadeIn(400);
	}
	else if(index == 3)
	{
 	 $(this).delay(4000);
 	 $(this).attr('display', '');
	 $(this).delay(1000).fadeIn(400);
	}
	if(notifshowed != ntotalnotif)
	{
 	 $(this).delay(4000).fadeOut(400);
  	 $(this).attr('display', 'none');
	}
	});
}


function countAllNotifications()
{
ntotalnotif += $('.warning').length;
ntotalnotif += $('.error').length;
ntotalnotif += $('.validation').length;
ntotalnotif += $('.success').length;
ntotalnotif += $('.notice').length;
ntotalnotif += $('.info').length;
ntotalnotif += $('.errorcont').length;
}

$(document).ready(function()
{
countAllNotifications();
// Ocultamos el los tabs si no tenemos h2
var nh2 = $('h2').length;
if(nh2 == 1) 
{
	//$('fieldset').append('<h2>Info</h2>');
}





function showAndHideMessages()
{


waiting = 0;
var nerrors = $('.warning').length;
if(nerrors > 0)
{
	setTimeout(
	  function() 
	  {
	    shower(".warning");
	    //do something special
	  }, waiting);
waiting+=3000;
}

var nerrors = $('.error').length;
if(nerrors > 0)
{
	setTimeout(
	  function() 
	  {
	    shower(".error");
	    //do something special
	  }, waiting);
waiting+=3000;
}
var nerrors = $('.validation').length;
if(nerrors > 0)
{
	setTimeout(
	  function() 
	  {
	    shower(".validation");
	    //do something special
	  }, waiting);
waiting+=3000;
}
var nerrors = $('.success').length;
if(nerrors > 0)
{
	setTimeout(
	  function() 
	  {
	    shower(".success");
	    //do something special
	  }, waiting);
waiting+=3000;
}
var nerrors = $('.notice').length;
if(nerrors > 0)
{
	setTimeout(
	  function() 
	  {
	    shower(".notice");
	    //do something special
	  }, waiting);
waiting+=3000;
}

var nerrors = $('.info').length;
if(nerrors > 0)
{
	setTimeout(
	  function() 
	  {
	    shower(".info");
	    //do something special
	  }, waiting);
waiting+=3000;
}
var nerrors = $('.errorcont').length;
if(nerrors > 0)
{
	setTimeout(
	  function() 
	  {
	    shower(".errorcont");
	    //do something special
	  }, waiting);
waiting+=3000;
}
}

var pathname = window.location.pathname;

if(pathname.indexOf("showEvents.sh") == -1)
	showAndHideMessages();


$(document).ready(function() 
{
	var themeActive = $("#theme-active").val();

	if(themeActive == "simple")
	{
		$('#main').jScrollPane();
	}
});

});
*/


