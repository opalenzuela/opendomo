/* Async reentrant URL fetcher
 */
function ajaxObject (layer, url) {
   var that = this;
   var updating = false;
   this.callback = function() {}

   this.update = function(passData) {
      if (updating == true)
		    return false;
      updating = true;

      var AJAX = null;
      if (window.XMLHttpRequest) {
         AJAX = new XMLHttpRequest();
      } else {
         AJAX = new ActiveXObject("Microsoft.XMLHTTP");
      }
      if (AJAX == null) {
         alert ("Your browser does not support AJAX.");
         return false
      } else {
         AJAX.onreadystatechange = function() {
            if (AJAX.readyState == 4 || AJAX.readyState == "complete") {
               LayerID.innerHTML = AJAX.responseText;
               delete AJAX;
               updating = false;
               that.callback();
            }
         }

         AJAX.open ("GET", urlCall, true);
         AJAX.send (null);
         return true;
      }
   }

   // This area set up on constructor calls.
   var LayerID = document.getElementById(layer);
   var urlCall = url;
}

