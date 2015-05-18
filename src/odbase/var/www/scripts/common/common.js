
function loadAsync(filePath, callback){
	jQuery.ajax({
		type : "GET",
		url : filePath
	}).success(callback);		
}

function loadJSON(filePath) {
	// Load json file;
	try {
		var json = loadTextFileAjaxSync(filePath, "application/json");
		// Parse json
		return JSON.parse(json);
	} catch (){
		var o = {};
		return o;
	}
}   

function loadTXT(filePath) {
	try {
		// Load TXT file;
		var text = loadTextFileAjaxSync(filePath, "text/plain");
		return text;
	} catch(e) {
		return "";
	}
}   

function loadRAW(filePath) {
	try {	
		// Load TXT file;
		var raw = loadTextFileAjaxSync(filePath, "application/octet-stream");
		return raw;
	} catch(e) {
		return "";
	}
} 

// Load text with Ajax synchronously: takes path to file and optional MIME type
function loadTextFileAjaxSync(filePath, mimeType){
	console.log("Using deprecated function");
	try {
		var xmlhttp=new XMLHttpRequest();
		xmlhttp.open("GET",filePath,false);
		if (mimeType != null) {
			if (xmlhttp.overrideMimeType) {
				xmlhttp.overrideMimeType(mimeType);
			}
		}
		xmlhttp.send();
		if (xmlhttp.status==200)
		{
			return xmlhttp.responseText;
		}
		else {
			// TODO Throw exception
			return null;
		}
	} catch(e) {
		// Do nothing 
		return null;
	}
}


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


jQuery(function($) {
	//init_focus();
	
	if ($("#wizFirstConfigurationStep1").length >0) $("#header").hide();
	
	/* BOTONES ON OFF - DEPRECATED in 2.2 */
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
});


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
	//FIXME Use jQuery instead	
	list.parentNode.insertBefore(tbar,list);
}

function sel_view_mode(list,mode){
	//FIXME Use jQuery instead
	list.parentNode.parentNode.childNodes[4].className=mode;
}

function isValid(ctrl,regexp){
	console.log("Invoking deprecated function isValid");
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


