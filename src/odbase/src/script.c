/*****************************************************************************
 *  This file is part of the OpenDomo project.
 *  Copyright(C) 2015 OpenDomo Services SL
 *  
 *  Daniel Lerch Hostalot <dlerch@opendomo.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/
/** 
  @file script.c
  @brief Script processing functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <libgen.h>
#include <src/match.h>
#include <sys/wait.h>

#include "conf.h"
#include "script.h"
#include "secfunc.h"
#include "i18n.h"
#include "src/str.h"
#include "src/string.h"

#define ODCGI_TEXT_HELP "Help"	///< Help caption
#define ODCGI_TEXT_BACK "Back"	///< Back caption
#define ODCGI_TEXT_ERROR_HELP "The operation was not completed"	///< Error text
#define ODCGI_TEXT_ERROR_HELP_LINK "More information"	///< Help caption
#define ODCGI_TEXT_ERROR_TRACKER_LINK "Notify the error"	///< Bugtracker caption
#define ODCGI_URL_HELP "http://www.opendomo.com/wiki/"	///< URL prefix to the help
/// URL to the discussion board
#define ODCGI_URL_TRACKER "https://github.com/opalenzuela/opendomo/issues/new"

/// Return TRUE if the first 3 bytes of the string matches the parameters
#define SPELL3(str,c1,c2,c3) ((str[0]==c1) && (str[1]==c2) && (str[2]==c3))
/// Return TRUE if the first 4 bytes of the string matches the parameters
#define SPELL4(str,c1,c2,c3,c4) (SPELL3(str,c1,c2,c3) && (str[3]==c4))
/// Return TRUE if the character is alphanumeric
#define ISALNUM(c) ((c>47 && c<58) || (c>64 && c<91) || (c>97 && c<122))

#define TRUE 1
#define FALSE -1

/// i18n translation function 
#define T(text) i18n(text, lang, TRUE)
/// i18n CLEAN translation function
#define CT(text) i18n(text, lang, FALSE)

#define html 1			///< HTML4
#define xml 2			///< Valid XML

extern char lang[8];		///< Language used
extern int gui;			///< Type of GUI used (XML, HTML, ...)
char legend[100] = "";		///< Legend
char script_name[256];		///< Nombre del script, p.e. "browseFiles.sh"
char cmdid[512] = "";		///< ID del comando, p.e.    "browseFiles"
int sepopen = 0; 			///< Do we have an open separator?


/// Wrapper function for split_line_to_cells
#define split_line(line,c1,c2,c3,c4,c5) split_line_to_cells(line,c1,sizeof(c1),c2,sizeof(c2),c3,sizeof(c3),c4,sizeof(c4),c5,sizeof(c5))
// {{{ split_line_to_cells()
/// Split a line in up to 5 cells, separated by tabulations
int
split_line_to_cells (const char *linia, char *col1, size_t col1_len, char *col2,
		     size_t col2_len, char *col3, size_t col3_len, char *col4,
		     size_t col4_len, char *col5, size_t col5_len)
{
  int i;
  char *str = (char *) linia;

  string_vector_t *vs = string_vector_alloc ();
  string_split (vs, str, "\t");

  for(i = 0; i < vs->size; i++)
  {
    switch (i)
    {
    case 0:
      snprintf (col1, col1_len, "%s", vs->v[i]->str);
      break;

    case 1:
      snprintf (col2, col2_len, "%s", vs->v[i]->str);
      break;

    case 2:
      snprintf (col3, col3_len, "%s", vs->v[i]->str);
      break;

    case 3:
      snprintf (col4, col4_len, "%s", vs->v[i]->str);
      break;

    case 4:
      snprintf (col5, col5_len, "%s", vs->v[i]->str);
      break;
    }
  }
  string_vector_free (vs);

  return vs->size;
}

// }}}

// {{{ script_format_error()
/// Format the error string
void
script_format_error (char *script, char *message)
{
  if(gui == html)
  {
    printf ("<div class='debug'>%s:%s</div>\n", script,
	    message);
  }
}

// }}}

// {{{ free_results()
/// Free results structure
void
free_results (char **results, size_t len)
{
  int i;
  for(i = 0; i < len; i++)
    if(results[i])
      free (results[i]);
}

// }}}

// {{{ script_get_valid_id()
/// Convert a string into a XML-valid ID
void
script_get_valid_id (char *name, char *id)
{
  int i;
  int j = 0;
  for(i = 0; i < strlen (name); i++)
  {
    //FIXME It's losing the first byte!
    if(ISALNUM (name[i]))
    {
      id[j] = name[i];
      j++;
      id[j] = 0;
    }
  }
}

// }}}

// {{{ script_process_comments()
/// Process a comments line and format the output in HTML or XML
void
script_process_comments (const char *buf)
{
	char tipname[50];
        char *classname = NULL;
        char *contents = NULL;

	if(strlen (buf) < 3) return;	// Avoid empty lines
	
	if(gui == html)
	{
		if((buf[0] == '-') && (buf[1] == '-'))
		{
			printf ("\t<hr/>\n");
		}	
		else if(SPELL3 (buf, 'E', 'R', 'R'))
		{
			printf ("\t<p class='error'>%s</p>\n", T (buf += 4));
			fflush (stdout);
		}
		else if(SPELL3 (buf, 'W', 'A', 'R'))
		{
			printf ("\t<p class='warning'>%s</p>\n", T (buf += 5));
			fflush (stdout);
		}
		else if(SPELL3 (buf, 'I', 'N', 'F'))
		{
			printf ("\t<p class='info'>%s</p>\n", T (buf += 5));	
		}
		else if(SPELL3 (buf, 'T', 'I', 'P'))
		{
			printf ("\t<p class='tooltip tip'>%s</p>\n", T (buf += 4));	
		}		
		else if(SPELL3 (buf, 'L', 'O', 'A'))
		{
			// The page might be slow at loading. Display a "loading" banner ...
			printf ("\t<p class='loading'>%s</p>\n", T(buf += 8));
			// ... and hide it when page is loaded.
			printf ("<script type='text/Javascript'>\n"\
				"$(function($){\n"\
				"	$('p.loading').hide() ;\n"\
				"});\n"\
				"</script>\n");
		}
		else if(SPELL3 (buf, 'U', 'R', 'L'))
		{
			printf ("\t<p class='link'>"
			 "<a href='%s' target='_blank'>%s</a></p>\n", buf += 4, buf);
		}		 
		else if(buf[0] == '>')
		{
			sstrncpy (legend, T (buf += 1), sizeof (legend));
		}
		else
		{
			// intentamos obtener la clase
			if(buf[0] == ' ')
			{
				printf ("\t<p class='comments'>%s</p>\n", T (buf += 1));
			}
			else
			{
                                classname = strtok (buf, " ");
                                contents = strtok (NULL, "\n");

				printf ("\t<p class='%s'>%s</p>\n", classname, T (contents));
			}
		}
	}	
	else
	{ /** XML OUTPUT **/
		if(SPELL3 (buf, 'E', 'R', 'R') || SPELL3 (buf, 'e', 'r', 'r'))
		{
			printf ("\t<error description='%s'/>\n", CT (buf += 4));
			fflush (stdout);
		}
		else if(SPELL3 (buf, 'W', 'A', 'R') || SPELL3 (buf, 'w', 'a', 'r'))
		{
			printf ("\t<warning description='%s'/>\n", CT (buf += 5));
			fflush (stdout);
		}
		else if(SPELL3 (buf, 'I', 'N', 'F') || SPELL3 (buf, 'i', 'n', 'f'))
			printf ("\t<info description='%s'/>\n", CT (buf += 5));
		else if(buf[0] == '>')
			sstrncpy (legend, T (buf += 1), sizeof (legend));
		else
			printf ("\t<message description='%s'/>\n", CT (buf += 1));
	}
}

// }}}

// {{{ script_process_categories_header()
/// Process the categories header and format the output in HTML
void
script_process_categories_header ()
{
  printf ("\n\t<ul id='categ' class='categories'>\n");
}

// }}}
// {{{ script_process_categories_body()
/// Process the categories and format the output in HTML
void
script_process_categories_body (char *code, char *name, char *type, char *extra)
{
	char id[20];
	for(int i = 0; i < 10; i++)
	{
		if(code[i] == '/')
			id[i] = 0;
		else
			id[i]=code[i];
	} 
	printf ("\t\t<li id='cat-%s' class='%s'>", id, type);
	printf ("<a href='" OD_URI "/%s'>", code);
	printf ("%s</a></li>\n", T (name));
}

// }}}
// {{{ script_process_categories_footer()
/// Process the categories footer and format the output in HTML
void
script_process_categories_footer ()
{
  printf ("\t</ul>\n");
}

// }}}

// {{{ script_process_form_header()
/// Process the "form" section and format the output in XML and HTML
void
script_process_form_header (char *id, char *legend, char *type)
{
	//char * encode ="text/plain";
	char *def_type = "";
	if(type == NULL)
		type = def_type;  
	if(gui == html)
	{
		printf ("\t<div id='%s_div' class='tabbertab'>\n", id);
// This hack does not work; characters sent don't pass the regex validation
//		if (id[0]=='w' && id[1]=='i' && id[2]=='z') { 
//			printf ("\t<form method='GET' ");			
//		} else {
			printf ("\t<form method='POST' ");
//		}
		printf (" id='%s_frm' action='%s.sh' accept-charset='UTF-8' "\
			"enctype='application/x-www-form-urlencoded'>\n", id, id);
		printf ("\t\t<fieldset class='tabform %s'>\n", type);
		if(legend[0] != 0)
		{
			printf ("\t\t<h2 id='%s_lgnd_h2'>%s</h2>\n", id, legend);
			printf ("\t\t<legend id='%s_lgnd'>%s</legend>\n", id, legend);
		}
		printf ("\t\t\t<ul id='%s'>\n", id);
	}
	else /** XML OUTPUT **/
	{
		printf ("\t<gui action='%s.sh' title='%s'>\n", id, legend);
	}
}

// }}}
// {{{ script_process_form_footer()
/// Process the "form" footer and format the output in XML and HTML
void
script_process_form_footer (char *cmdid)
{
  // Esto se llama después de mostrar un formulario SIN ACTIONS definidas
  if(gui == html)
  {
    printf ("\t\t\t</ul>\n");
    printf ("\t\t\t<div class='toolbar' id='%s_tbr'>\n", cmdid);
    printf ("\t\t\t\t<button type='submit'>%s</button>\n", CT ("Submit"));
    //printf("\t\t\t\t<button type='reset'>%s</button>\n", CT("Reset"));
    //printf("\t\t\t\t<button type='button' "
    //     "onclick='history.back()'>%s</button>\n", CT(ODCGI_TEXT_BACK));
    printf ("\t\t\t\t<button type='button' "
	    "onclick=\"help('%s');\">%s</button>\n", cmdid, CT (ODCGI_TEXT_HELP));
    printf ("\t\t\t</div>\n");
    printf ("\t\t</fieldset>\n" "\t</form></div>\n");
  }
  else /** XML OUTPUT **/
  {
    printf ("\t</gui>\n");
  }
}

// }}}

// {{{ script_process_list_header()
/// Process the "list" section and format the output in XML and HTML
void
script_process_list_header (char *id, char *title, char *type)
{
	if(gui == html)
	{
		if(type[0] == 0) strcpy(type,"listview");

		// Soporte Multi-formulario
		if(strcmp (type, "simple") != 0) 
		{
			printf ("\t<div class='tabbertab' id='%s_div'>\n", id);
		} else {
			printf ("\t<div id='%s_div'>\n", id);
		}
		
		printf ("\t<form id='%s_frm' action='%s.sh' method='POST'>\n", id, id);
		printf ("\t\t<fieldset class='%s'>\n", type);
		if(title[0] != 0)
		{
			printf ("\t\t<h2 id='%s_lgnd_h2'>%s</h2>\n", id, legend);
			printf ("\t\t\t<legend id='%s_lgnd'>%s</legend>\n", id, title);
		}
		printf ("\t\t\t<ul id='%s'>\n", id);
	} else { /** XML OUTPUT **/
		printf ("\t<gui action='%s.sh' title='%s'>\n", id, title);
	}
}

// }}}
// {{{ script_process_list_body()
/// Process the "list" lines and format the output in XML and HTML
void
script_process_list_body (char *id, char *desc, char *type, char *extra)
{
  char link[256] = "#  ";
  char img[1024] = "";
  char file_no_ext[1024] = "";
  char jpg_image[1024] = "";

  char *t;

  t = type;
  
	//Particular case: if ID starts by ":", render as info text line
  if (id[0] == ':') {
	printf("\t\t\t\t<li class='par'>%s</li>\n", 
		T(desc));
  	return;
  }

  if(id[0] == '-')
  {
    id++;
    snprintf (link, sizeof (link), "%s?odcgioptionsel[]=%s", script_name, id);
  }
  else if(id[0] == '/')
  {
    snprintf (link, sizeof (link), "%s", id);
    id++;
    id = strtok (id, "/.");
  }
  else if(id[0] == '.')
  {
    snprintf (link, sizeof (link), "%s", id);
    id += 2;
    id = strtok (id, "/.");
  }
  else
  {
    snprintf (link, sizeof (link), "#");	// By default, no link is built
  }

  if(SPELL4 (type, 'v', 'i', 'd', 'e'))
  {
    // Smash the extension
    for(int i = 0; i < strlen (id); i++)
    {
		if(id[i] == '.') id[i] = 0;
    }

    snprintf (link, sizeof (link), "/cgi-bin/od.cgi?GUI=none&amp;FILE=%s", id);

    printf ("<li id='%s'>"
	    "<a href='%s.webm'><b>%s</b></a><br/>\n"
	    "<video id='%s' class='%s' controls='controls' poster='%s.jpg'>"
	    "\t<source src='%s.mp4' type='video/mp4' />\n"
	    "\t<source src='%s.webm' type='video/webm' />\n"
	    "\tYour browser does not support the video tag.\n"
	    "</video></li>\n",  id, link, desc, id, type, link, link, link);
    return;
  }

  else if(strstr (type, "separator"))
  {
	printf ("\t\t\t\t<div id='%s_sep' class='separator' >"
		"<label id='%s_lbl'>%s</label>\n\t\t\t\t\t",
		id, id, T (desc));  	
  	printf("<p>&nbsp;</p></div>\n");
	return;
  }  
  
  else if(SPELL4 (type, 's', 't', 'r', 'e'))	// stream
  {
    printf ("<li id='%s' class='%s'><b>%s</b><br/>", id, type, desc);
    printf ("<a href='?option[]=%s'>\n", id);
    printf ("<img name='%s' src='%s' alt='%s' class='video %s' "
	    "onerror='tryToReloadCam(this);'/><br/>",
	    id, extra, desc, type);
    printf ("</a>");

    /*
       printf("<p class='streambuttons'>");
       if (strstr(type,"recording")){
       printf("<a class='record' href='%s?command=%s&value=off'>Stop</a>\n", script_name, id);
       } else {
       printf("<a class='record' href='%s?command=%s&value=on'>Record</a>\n", script_name, id);
       }
       printf("<a class='zoom' href='javascript:fullScreen(\"%s\");'>Zoom</a>\n", id);
       printf("</p>");
     */
    printf ("</li>\n");
    return;
  }
  else if(SPELL4 (type, 'f', 'i', 'l', 'e'))
  {
    /* Comprobamos que sea un mjpg */
    if(strstr (id, ".mjpg"))
    {
      str_assign_str (file_no_ext, sizeof (file_no_ext), basename (id));
      str_cut (file_no_ext, sizeof (file_no_ext), 0, strlen (file_no_ext) - 5);

      //snprintf(link,sizeof(link),"/cgi-bin/od.cgi?play_mjpg=%s",file_no_ext);
      snprintf (link, sizeof (link),
		"/cgi-bin/od.cgi/config/video/player.sh?option[]=%s",
		file_no_ext);
    }
    else
    {
      snprintf (link, sizeof (link), "%s?FILE=%s&amp;GUI=none",
		script_name, id);
    }

    // En los tipos "file?image" enlazamos a la API del CGI para ficheros
    if((t[5] == 'i') && (t[6] == 'm') && (t[7] == 'a') && (t[8] == 'g')
       && (t[9] == 'e'))
    {
      snprintf (img, sizeof (img),
		"<img alt='%s' src='%s?FILE=%s&amp;GUI=none'/>", id,
		script_name, id);
    }
    else if(strstr (id, ".mjpg"))
    {
      str_assign_str (jpg_image, sizeof (jpg_image), id);
      str_replace (jpg_image, strlen (jpg_image), ".mjpg", ".jpg");
      //TODO: we first check if the file exists
      snprintf (img, sizeof (img),
		"<img alt='%s' src='%s?FILE=%s&amp;GUI=none'/>", jpg_image,
		script_name, jpg_image);

    }
  }
  if(SPELL4 (type, 'i', 'm', 'a', 'g'))
  {
    if(extra != NULL)
    {
      snprintf (img, sizeof (img), "<img alt='%s' src='%s'/>", id, extra);
      //snprintf (link, sizeof (link), "%s", extra);
    }
  }
  if(gui == html)
  {
    //TODO Remove all non-alphabetic chars of id
    printf ("\t\t\t\t<li id='%s' class='%s' >", id, type);
	printf ("<a href='%s'>", link);
    //printf ("<div class='imgframe'>%s</div>", img);
    printf ("<b id='%s_lbl'><input type='checkbox' name='option[]' value='%s'/>",
    	id, id);
    printf ("%s</b></a>", T (desc));
    if(extra != NULL)
    {
		printf ("\n<!-- extra= %s -->\n", extra);
		extra = T(extra);
		printf ("\n<!-- t(extra)= %s -->\n", extra);
		//printf ("<div class='imgframe'>%s</div>", img);
		printf ("<p>%s</p></li>\n", extra);
    }
    else
    {
		printf ("<p>&nbsp;</p></li>\n");
    }
  }
  else
  {/** XML OUTPUT **/
    if(strchr (link, '?') == NULL)
    {
		printf ("\t\t<item name='%s' href='%s?GUI=XML' label=\"%s\" class='%s'/>\n", 
			id, link, CT (desc), extra);
    }
    else
    {
		printf ("\t\t<item name='%s' href='%s&amp;GUI=XML' label=\"%s\" class='%s' />\n",
			id, link, CT (desc), extra);
    }
  }
}

// }}}
// {{{ script_process_list_footer()
/// Process the "list" footer and format the output in XML and HTML
void
script_process_list_footer ()
{
// Esto se llama después de mostrar un listado SI NO HAY ACTIONS definidas
  if(gui == html)
  {
    printf ("\t\t\t</ul>\n" "\t\t</fieldset>\n" "\t</form></div>\n");
  }
  else
  {
    printf ("\t</gui>\n");
  }
}

// }}}

// {{{ script_process_action_header()
/// Process the "action" section and format the output in XML and HTML
void
script_process_action_header (char *cmdid)
{
  if(gui == html)
  {
    printf ("\n\t\t\t<div class='toolbar' id='%s_tbr'>\n", cmdid);
  }
  else
  {
    printf ("\t\t<actions>\n");
  }
}

// }}}
// {{{ script_process_action_body()
/// Process the "action" lines and format the output in XML and HTML
void
script_process_action_body (char *id, char *name, char *type)
{
	char *cmdid;
	sstrncpy(cmdid, id, sizeof (cmdid));
	for(int i = 0; i < strlen(cmdid); i++)
	{
		if(cmdid[i] == '.')	cmdid[i] = 0;
	}
	
	if(gui == html)
	{
		if(strstr (id, "goback")) {
			printf ("\t\t\t<button type='button' "\
				"onclick='history.back();'>%s</button>\n",
				T(name));	
		} else {
			// With the name of the field we can know the target script:
			printf ("\t\t\t<button type='submit' id='submit-%s' name='submit_%s' "\
	    		" formaction='/cgi-bin/od.cgi/%s'>%s</button>\n",
				cmdid, id, id, T(name));
		}
	}
	else
	{/** XML OUTPUT **/
		printf ("\t\t\t<action id='%s' description='%s' />\n", 
			id, CT (name));
	}
}

// }}}
// {{{ script_process_action_footer()
/// Process the "actions" footer and format the output in XML or HTML
void
script_process_action_footer (char *cmdid)
{
  if(gui == html)
  {
//TODO Añadir la sección <noscript>
    //printf("\t\t\t\t<button type='button' "
    //     "onclick='history.back()'>%s</button>\n", CT(ODCGI_TEXT_BACK));
    printf ("\t\t\t\t<button type='button' "
	    "onclick=\"help('%s');\">%s</button>\n", cmdid, T (ODCGI_TEXT_HELP));
    printf ("\t\t\t</div>\n" "\t\t</fieldset>\n" "\t</form></div>\n");
  }
  else
  {/** XML OUTPUT **/
    printf ("\t\t</actions>\n");
  }
}

// }}}

// {{{ script_process_graph_header()
void
script_process_graph_header (char *id, char *legend, char *type)
{
	char *def_type = "lines";
	char series[600];
	char xaxis[600];
	char modifier[500];

	
	if(gui == html)
	{
		printf ("\t<div id='%s_div'>\n", id);
		printf ("\t<form id='%s_frm' action='%s.sh' method='POST'>\n", 
			id, id);
		printf ("\t\t<fieldset>\n");
		if(legend[0] != 0)
		{
			printf ("\t\t<legend id='%s_lgnd'>%s</legend>\n", id, legend);		
		}
		if(type == NULL) type = def_type;
		
		printf ("\t\t\t<div id='%s' class='graph %s'></div>\n", id, type);
		printf ("<script type='text/javascript'>\n"
			"var %s_func = function () {\n"
			"	var data=[];\n",id);
			
			
		// General type for the graphic (bars/pie/lines)
		if (strstr(type, "bars")){
			snprintf(series, sizeof(series), "series: { "\
				"bars: { show: true, barWidth: 0.4, align: 'center'}"\
				" },"\
				"grid: { hoverable: true, clickable: true },"\
				"xaxis: { tickSize:1, tickDecimals:0 } ");
		} else if (strstr(type, "piled_bars")){
			snprintf(series, sizeof(series), "series: { "\
				"lines: { show: false, steps: false },"\
				" bars: { show: true, barWidth: 0.4, align: 'center', series_spread: true }"\
				" },"\
				"grid: { hoverable: true, clickable: true },"\
				"xaxis: { tickSize:1, tickDecimals:0 } ");
		} else if(strstr(type, "pie")){
			snprintf(series, sizeof(series), "series: { "\
				"pie: { show: true, radius: 110, tilt:0.7,label: { show: true, radius: 1,  formatter: function(label, series){ return '<div style=\"font-size:8pt;text-align:center;padding:2px;color:white;font-weight:700;\">'+label+'<br/>'+Math.round(series.percent)+'%%</div>';}, background: { opacity: 0.8 }} }"\
				" },"\
				" legend : {"\
				"	show:false"\
				" },"\
				"grid: { hoverable: true, clickable: true } ");			
		} else { // Lines and points is the default
			snprintf(series, sizeof(series), "series: {"\
				"lines: { show: true }, "\
				"points: { show: true } "\
				"},"\
				"grid: { hoverable: true, clickable: true },"\
				"xaxis: { tickSize:1, tickDecimals:0 } ");
	    	}
	    	
	    	// More data about the axis
		if (strstr(type, "minutely")){
			snprintf(xaxis, sizeof(xaxis), ", xaxis: { "\
				"mode: \"time\",  "\
				"minTickSize: [1, \"minute\"] "\
				" }");	
			if(strstr(type, "bars") || strstr(type, "piled_bars"))
			{
				snprintf(modifier,sizeof(modifier),
				"options[\"series\"][\"bars\"][\"barWidth\"] = 60000; \
				 options[\"yaxis\"] = {}; \
				 options[\"yaxis\"][\"min\"] = 0 \
				");	
			}
			else
			{
				snprintf(modifier,sizeof(modifier),
				 "options[\"yaxis\"] = {}; \
				 options[\"yaxis\"][\"min\"] = 0 \
				");	
			}
		} 
		else if (strstr(type, "hourly")){
			snprintf(xaxis, sizeof(xaxis), ", xaxis: { "\
				"mode: \"time\",  "\
				"minTickSize: [1, \"hour\"] "\
				" }");	
			if(strstr(type, "bars") || strstr(type, "piled_bars"))
			{
				snprintf(modifier,sizeof(modifier),
				"options[\"series\"][\"bars\"][\"barWidth\"] = 3600000; \
				 options[\"yaxis\"] = {}; \
				 options[\"yaxis\"][\"min\"] = 0 \
				");	
			}
			else
			{
				snprintf(modifier,sizeof(modifier),
				 "options[\"yaxis\"] = {}; \
				 options[\"yaxis\"][\"min\"] = 0 \
				");	
			}
		} 
		else if (strstr(type, "daily")){
			snprintf(xaxis, sizeof(xaxis), ", xaxis: { "\
				"mode: \"time\",  "\
				"minTickSize: [1, \"day\"] "\
				" }");	
			if(strstr(type, "bars") || strstr(type, "piled_bars"))
			{
				snprintf(modifier,sizeof(modifier),
				"options[\"series\"][\"bars\"][\"barWidth\"] = 86400000; \
				 options[\"yaxis\"] = {}; \
				 options[\"yaxis\"][\"min\"] = 0 \
				");	
			}
			else
			{
				snprintf(modifier,sizeof(modifier),
				 "options[\"yaxis\"] = {}; \
				 options[\"yaxis\"][\"min\"] = 0 \
				");	
			}
		} 
		else if (strstr(type, "monthly"))
		{
			snprintf(xaxis, sizeof(xaxis), ", xaxis: { "\
				"mode: \"time\",  "\
				"minTickSize: [1, \"month\"] "\
				" }");
			if(strstr(type, "bars") || strstr(type, "piled_bars"))
			{
				snprintf(modifier,sizeof(modifier),
				"options[\"series\"][\"bars\"][\"barWidth\"] = 24*60*60*1000*25; \
				 options[\"yaxis\"] = {}; \
				 options[\"yaxis\"][\"min\"] = 0 \
				");	
			}
			else
			{
				snprintf(modifier,sizeof(modifier),
				 "options[\"yaxis\"] = {}; \
				 options[\"yaxis\"][\"min\"] = 0 \
				");	
			}

		}
		else 
		{
			snprintf(xaxis, sizeof(series), " ");
			snprintf(modifier,sizeof(modifier)," ");	
		}
	    	
		// Finally, printing the data:    	
		printf(  "\tvar options = {%s %s }; %s\n", series, xaxis, \
								modifier);
	    	
	}
	else
	{
	// Not usable for XML
	}
}

// }}}

// {{{ script_process_graph_body()
void
script_process_graph_body (char *id, char *name, char *type, char *extra)
{
	if(gui == html)
	{
		// With the name of the field we can know the target script:
		printf ("\tvar plot_%s = {data: %s, label: \"%s\"};\n", id, type, name);
		//TODO: Transform only if "date" (or non-numerical) type
		printf("\tfor (var j=0; j< plot_%s.data.length; j++){ \n"\
			"\t\tvar d = plot_%s.data[j][0];\n"
			"\t\tif (isNaN(d)) {\n"\
			"\t\t\tvar dt_flot = new Date(d);\n"\
			"\t\t\ttimeOffsetInHours = (dt_flot.getTimezoneOffset()/60) * (-1);\n"\
			"\t\t\tdt_flot.setHours(dt_flot.getHours() + timeOffsetInHours);\n"\
			"\t\t\tplot_%s.data[j][0] = dt_flot.getTime();\n"\
			"\t\t}\n"\
			"\t}\n",id, id, id);
		printf("\tdata.push(plot_%s);\n", id);
	}
	else
	{
		// Not usable for XML
	}
}

// }}}

// {{{ script_process_graph_footer()
void
script_process_graph_footer (char *cmdid, int closeblock)
{
	if(gui == html)
	{
		for(int i = 0; i < strlen (cmdid); i++)
		{
		if(cmdid[i] == '.')
			cmdid[i] = 0;
		}
		printf ("\t$.plot($(\"#%s\"), data, options);\n", cmdid);
		printf ("\n}\nsetTimeout(function(){%s_func();}, 1000);\n\t\t\t</script>\n",cmdid);
		printf ("\t\t<br/>\n" );
		if (closeblock==TRUE)
			printf ("</fieldset>\n\t</form></div>\n");
	}
	else
	{
	// Not usable for XML
	}
}

// }}}

char *
remove_brackets (const char *originalstring, char *target)
{
  char *fb;
  char *lb;
  //char os[1024];

  // Miramos si el listado contiene un elemento "list[item1,item2...] etc"...
  fb = strchr (originalstring, '[');
  if(fb == NULL)
  {				// No se ha encontrado "[", por tanto
    strncpy (target, originalstring, 100);	// salimos (devolvemos la cadena inicial)
    //return originalstring;
  }
  else
  {
    int k = (fb - originalstring + 0);	// Sí se ha encontrado
    strncpy (target, originalstring, k);	// copiamos el texto previo a "["
//    strncat(target, ",", 1);

    // Ya solamente falta localizar "]" 
    // y copiar en "target" lo que venga a continuación (" etc")
    //FIXME: Por alguna razón, en ciertos casos provoca segfaults
//    strncpy(os,originalstring,1024);
    lb = strrchr (originalstring, ']');
    strcat (target, (char *) (lb + 1));
  }
  return target;
}

// {{{ script_process_form_body()
/// Process the "form" section and format the output in HTML
int
script_process_form_body (char *id, char *desc, char *type, char *pextra)
{
  int i = 0;
  char rule[1024];
  char buffer[20] = "";
  char *extra = "";
  char *pch = NULL;
  char stype[100];

  char *cleantype;

  cleantype = (char *) malloc (100);
  memset (cleantype, 0, 100);
  cleantype = remove_brackets ((const char *) type, cleantype);

  for(i = 0; i < 100; i++)
  {
    if((type[i] > 96 && type[i] < 123) || type[i] == '.')
    {
      stype[i] = type[i];
      stype[i + 1] = 0;
    }
    else
    {
      break;
    }
  }
  //char *ttype;

	if(pextra != NULL)
		extra = pextra;
 	// ********** LINE HEADER **************
 	printf ("\n\n<!-- %s [%s] %s %s !!%s!! (%s) -->\n",
		stype, buffer, id, desc, extra, cleantype);
 	syslog (LOG_DEBUG, "\n\n<!-- %s [%s] %s %s !!%s!! (%s) -->\n",
		stype, buffer, id, desc, extra, cleantype);

	//Particular case: if ID starts by ":", render as info text line
	//TODO: Remove this and replace it by # information paragraph
	if(id[0]==':'){
		printf("\t\t\t\t<li class='par'>%s</li>\n", T(desc));
		return 0;
	}
	
  // ********** END HEADER *****************

  if(strstr (stype, "password"))
  {
	printf ("\t\t\t\t<li id='%s_li' class='%s'>"
		"<label id='%s_lbl' for='%s'><b>%s</b></label>\n\t\t\t\t\t",
		id, cleantype, id, id, T (desc));
	printf ("<p><input id='%s' name='%s' type='password' value=\"%s\"/></p></li>\n",
		id, id, extra);
  }

  else if(strstr (stype, "readonly"))
  {
    /*
       printf("<p title='%s'>"\
       "<input name='%s' type='hidden' value=\"%s\"/>"\
       "<span class='%s'>%s</span></p>"\
       "</li>\n",
       extra, id, extra, extra,extra);
     */
	printf ("\t\t\t\t<li id='%s_li' class='%s' >"
		"<label id='%s_lbl' for='%s'><b>%s</b></label>\n\t\t\t\t\t",
		id, cleantype, id, id, T (desc));     
	printf ("<p><input id='%s' name='%s' type='text' readonly='readonly' value=\"%s\"/></p>"
		"</li>\n", id, id, extra);
  }

  else if(strstr (stype, "longtext"))
  {
	printf ("\t\t\t\t<li id='%s_li' class='%s' >"
		"<label id='%s_lbl' for='%s'><b>%s</b></label>\n\t\t\t\t\t",
		id, cleantype, id, id,  T (desc));  
	//FIXME Open brackets for long text input, and return "1"
	printf ("<br/><textarea name='%s' id='%s'>%s</textarea></li>\n", 
		id, id, extra);
  }

  else if(strstr (stype, "separator"))
  {
  	sepopen=1;
	printf ("\t\t\t\t<div id='%s_sep' class='separator' >"
		"<label id='%s_lbl'>%s</label>\n\t\t\t\t\t",
		id, id, T (desc));  	
  	printf("<p>&nbsp;</p></div>\n");
  }

  else if(strstr (stype, "list"))
  {
	printf ("\t\t\t\t<li id='%s_li' class='%s' >"
		"<label id='%s_lbl' for='%s'><b>%s</b></label>\n\t\t\t\t\t",
		id, cleantype, id, id, T (desc));  
	printf ("<p><select id='%s' name='%s' class='list'>\n", id, id);
	if(parse_list (type, extra) != 0)
	{
		printf ("<optgroup label='Invalid data'></optgroup>\n");
	}
	printf ("\t\t\t\t  </select></p></li>\n");
  }

  else if(strstr (stype, "boolean"))
  {
	printf ("\t\t\t\t<li id='%s_li' class='%s' >"
		"<label id='%s_lbl' for='%s'><b>%s</b></label>\n\t\t\t\t\t<p>",
		id, cleantype, id, id, T (desc));  
	if (strstr(extra,"on") || strstr(extra,"true") ) {
		printf ("<input name='%s' id='%s' type='checkbox' checked='checked'>", 
			id, id);
	} else {
		printf ("<input name='%s' id='%s' type='checkbox'>", id, id);	
	}
	printf("</p></li>\n");
  }
  
  else if(strstr (stype, "range"))
  {
	printf ("\t\t\t\t<li id='%s_li' class='%s' >"
		"<label id='%s_lbl' for='%s'><b>%s</b><span class='preview'></span></label>\n\t\t\t\t\t",
		id, cleantype, id, id, T (desc));  
	printf ("<p><input name='%s' id='%s' type='range' value='%s'></p></li>\n", 
		id, id, extra);
  }  
  
  else if(strstr (stype, "subcommand"))
  { //TODO remove SUBCOMMANDS (deprecated)
	printf ("\t\t\t\t<li id='%s_li' class='%s' >"
		"<label id='%s_lbl' for='%s'><b>%s</b></label>\n\t\t\t\t\t",
		id, cleantype, id, id, T (desc));  
	if(strcmp (type, "subcommand[on,off]") == 0)
	{
		printf ("<p id='%s' title='%s' class='DO %s'>", id, extra, extra);
	}
	else if(strncmp (type, "subcommand[0", 12) == 0)
	{
		printf ("<p class='slider'>");
	}
	else
	{
		printf ("<p class='buttons'>");
	}
	pch = strtok (type, "[],");
	pch = strtok (NULL, "[],");
	while(pch != NULL)
	{
		
		if(0 == strcmp (pch, extra))
		{
			printf ("<a id='cmd_%s_%s' href='%s?command=%s&value=%s' "
				" class='sw-%s ON'>%s</a>",
				id, pch, script_name, id, pch, pch, pch);
		}
		else
		{
			printf ("<a id='cmd_%s_%s' href='%s?command=%s&value=%s' "
				" class='sw-%s OFF'>%s</a>",
				id, pch, script_name, id, pch, pch, pch);
		}
		pch = strtok (NULL, "[],");
	}
	printf ("\t\t\t\t  </p></li>\n");
  }
  else if(strstr (stype, "file"))
  {
	printf ("\t\t\t\t<li id='%s_li' class='%s' >"
		"<label id='%s_lbl' for='%s'><b>%s</b></label>\n\t\t\t\t\t",
		id, cleantype, id, id, T (desc));
	printf ("<p><iframe name='fileupload-%s' class='fileupload' scrolling='auto' frameborder='no' "
		" src='/cgi-bin/fileupload.py?fname=%s'></iframe></p></li>\n",
		id, extra);
  }
  else if(strstr (stype, "image"))
  {
	printf ("\t\t\t\t<li id='%s_li' class='%s' >"
		"<label id='%s_lbl' for='%s'><b>%s</b></label>\n\t\t\t\t\t",
		id, cleantype, id, id, T (desc));
	printf ("<div class='imgframe'>"
		"<img id='imgframe_%s' class='imgfield' name='%s' src='%s' alt='%s'/></div></li>\n",
		id, id, extra, CT (desc));
  }

  else if(strstr (stype, "videofile"))
  {
	printf ("\t\t\t\t<li id='%s_li' class='%s' >"
		"<label id='%s_lbl' for='%s'><b>%s</b></label>\n\t\t\t\t\t",
		id, cleantype, id, id,  T (desc));
	printf ("<script>"
	    "var drag_dealer; "
	    "function %s_load_slider() {"
	    "	drag_dealer = new Dragdealer('%s_slider', { "
	    "	    steps: 100, "
	    "		snap: true, "
	    "		callback: function(x, y) { "
	    "			pos(x,\"%s\",\"%s\"); "
	    "		}" "	});" "}" "</script>\n", id, id, extra, id);

	printf ("<div class='imgvideofile'>");
	printf
      ("<img id='img_%s' onload='%s_load_slider()' name='%s' src='%s' alt='%s'/>",
       id, id, id, extra, CT (desc));

	printf ("<div id='%s_slider' class='video_slider'>"
	    "<div class='handle'></div></div>", id);
	printf
      ("<button class=\"stop_btn\" type=\"button\" onClick=\"stop_stream(\'%s\',\'%s\')\"/></button>",
       id, extra);
	printf
      ("<button class=\"play_btn\" type=\"button\" onClick=\"play_stream(\'%s\',\'%s\')\"/></button>",
       id, extra);
	printf
      ("<button class=\"pause_btn\" type=\"button\" onClick=\"window.stop()\"/></button>");

	printf ("</div></li>\n");
  }
  /* // Moved to list
     else if (strstr(stype,"stream"))
     {
     printf("<div class='imgstream_%s'>",id);

     if (strcmp(type,"stream[recording]") == 0)
     {
     printf("<img name='%s' src='%s' alt='%s' onerror='this.src=\"/images/nocam.png\";'/>", id, extra, CT(desc));
     printf("<p class='recbutton on'>");
     printf("<a href='%s?command=%s&value=off'>Rec</a></p>\n", script_name, id);
     printf("<p class='zoom'>");
     printf("<a href='javascript:fullScreen(\"%s\");'>Zoom</a></p>\n", id);
     }
     else
     {
     printf("<img name='%s' src='%s' alt='%s' onerror='this.src=\"/images/nocam.png\";'/>", id, extra, CT(desc));
     printf("<p class='recbutton off'>");
     printf("<a href='%s?command=%s&value=on'>Rec</a></p>\n", script_name, id);
     printf("<p class='zoom'>");
     printf("<a href='javascript:fullScreen(\"%s\");'>Zoom</a></p>\n",id);
     }

     printf("</div></li>\n");
     } */

  else if(strstr (stype, "video"))
  {
	printf ("\t\t\t\t<li id='%s_li' class='%s' >"
		"<label id='%s_lbl' for='%s'><b>%s</b></label>\n\t\t\t\t\t",
		id, cleantype, id, id, T (desc));  
	printf ("<div class='imgframe'>"
		"<embed name='%s' class='%s' src='%s' alt='%s' /></div>"
		"</li>\n", id, type, extra, CT (desc));
  }

  else if(strstr (stype, "application"))
  {
	printf ("\t\t\t\t<li id='%s_li' class='%s' >"
		"<!-- <label id='%s_lbl' for='%s'><b>%s</b></label> -->\n\t\t\t\t\t",
		id, cleantype, id, id, T (desc));  
	printf ("<iframe name='frm%s' scrolling='auto' frameborder='no' "
		"align='center' class='embeddedapp' src='%s'></iframe></li>\n",
		id, extra);
  }
  else if(strstr (stype, "date"))
  {
	printf ("\t\t\t\t<li id='%s_li' class='%s' >"
		"<label id='%s_lbl' for='%s'><b>%s</b></label>\n\t\t\t\t\t",
		id, cleantype, id, id, T (desc));  
	printf ("<p><input name='%s' id='id_%s' type='date' class='%s' value='%s' "\
		" onkeyup=\"isValid(this,'^[0-3][0-9]/[0-1][0-9]/20[0-9][0-9]$')\"/>"
		"</p></li>\n", id,id, type, extra);

  }
  else if(strstr (stype, "text"))
  {
	printf ("\t\t\t\t<li id='%s_li' class='%s' >"
		"<label id='%s_lbl' for='%s'><b>%s</b></label>\n\t\t\t\t\t",
		id, cleantype, id, id, T (desc));  
	if(type[4] == ':')		//text:[a-z0-9]
	{
		pch = type + 5;		// pch apunta a [
		//pch = strtok (type,"()\n");
		//pch = strtok (NULL, "()\n");
		//if (pch != NULL)
		//TODO Adjust selected format
		sprintf (rule, "onkeyup=\"isValid(this,'^%s$')\"", pch);
	}
	else
	{
		sprintf (rule, "");
	}
	printf ("<p><input id='%s' name='%s' type='text' class='%s' value='%s' %s/>"
		"</p></li>\n", id, id, type, extra, rule);
  }
  else //TODO: Merge with "text", as the default option
  {
	printf ("\t\t\t\t<li id='%s_li' class='%s' >"
		"<label id='%s_lbl' for='%s'><b>%s</b><span class='preview'></span></label>\n\t\t\t\t\t",
		id, cleantype, id, id, T (desc));  
	printf ("<p><input id='%s' name='%s' type='%s' class='%s' value='%s'/>"
		"</p></li>\n", id, id, type, type, extra);
  }
  free (cleantype);
  return 0;
}

// }}}

// {{{ script_process_form_body_xml() 
/// Process the "form" section and format the output in XML
void
script_process_form_body_xml (char *id, char *desc, char *type, char *pextra)
{
//   char rule[1024];
//   char buffer[20]="";
  char *extra = "";
  char *pch = NULL;
  //char *ttype;

  if(pextra != NULL)
    extra = pextra;
  if(strstr (type, "password"))
  {
    printf ("<password id='%s' label=\"%s\"/>\n", id, desc);
  }

  else if(strstr (type, "readonly"))
  {
    printf ("<text id='%s' readonly='true' label=\"%s\" value=\"%s\">%s</text>\n",
	    id, desc, pextra, pextra);
  }

  else if(strstr (type, "longtext"))
  {
    printf ("<text id='%s' label=\"%s\"/>\n", id, desc);
  }

  else if(strstr (type, "separator"))
  {
    // No imprimir nada más. Con "label" es suficiente
  }

  else if(strstr (type, "list"))
  {
    printf ("<list id='%s' value='%s'>\n", id, extra);
    pch = strtok (type, "[],\n");
    pch = strtok (NULL, "[],\n");

    printf("<!-- beginning %s - %s -->\n", pch, extra);

    while(pch != NULL)
    {
      if(0 == strcmp (pch, extra))
	printf ("\t\t\t\t\t<item selected='selected'>%s</item>\n", pch);
      else
	printf ("\t\t\t\t\t<item>%s</item>\n", pch);
      pch = strtok (NULL, "[],\n");
    }
    printf ("\t\t\t\t  </list>\n");
  }

  else if(strstr (type, "subcommand"))
  {
    printf ("\t\t\t\t\t<commands id='%s'>\n", id);
    pch = strtok (type, "[],\n");
    pch = strtok (NULL, "[],\n");
    while(pch != NULL)
    {
      //TODO Permitir múltiples valores seleccionados al mismo tiempo
      if(0 == strcmp (pch, extra))
      {
	printf ("\t\t\t\t\t\t<command url='%s?command=%s&amp;value=%s' "
		"state='on' label='%s'/>\n", script_name, id, pch, pch);
      }
      else
      {
	printf ("\t\t\t\t\t\t<command url='%s?command=%s&amp;value=%s' "
		" state='off' label='%s'>%s</command>\n",
		script_name, id, pch, pch, pextra);
      }

      pch = strtok (NULL, "[],\n");
    }
    printf ("\t\t\t\t  </commands>\n");
  }

  else if(strstr (type, "image"))
  {
    printf ("<img id='%s' src=\"%s\" alt='%s'/>\n", id, extra, T (desc));
  }

  else if(strstr (type, "video"))
  {
    printf ("<embed name='%s' class='%s' src='%s' alt='%s' "
	    "width='320' height='260'/>\n", id, type, extra, T (desc));
  }

  else if(strstr (type, "application"))
  {
    printf ("<iframe name='0' scrolling='auto' frameborder='no' "
	    "align='center' height='200' width='300' src='%s'></iframe>\n",
	    extra);
  }

  else
  {				// Text is the default field
    printf ("<text id='%s' label=\"%s\" value='%s'/>\n", id, desc, extra);
  }
}

// }}}

// {{{ script_exec()
/** Execute the command specified and format the output 
 \param cmd Command to execute
 \param section Section name
 \param env Environment variable structure
*/
int
script_exec (const char *cmd, const char *section, script_env_t * env)
{
  int submit2set = 1;
  char buf[10240] = "";
  ///< Comando completo, p.e.  "browseFiles.sh data"
  char command[strlen (cmd) + 10];
  char *lsid = NULL;
  char *lsname = NULL;
  char *lstype = NULL;
  char *lsextra = NULL;
  int usetabs = 0;

  openlog ("odcgi", LOG_PID, LOG_USER);
  /// @todo filter dangerous chars from cmd (".." , >, <, |, ., `, )
  snprintf (command, sizeof (command), "%s 2>&1", cmd);
  //if(command[strlen(command)-1]=='/') command[strlen(command)-1]=0;
  //strncat(command,sizeof(command)," 2>&1"); // Supress stderr

  int offset = 0;
  if(strlen (command) > 255)
    offset = strlen (command) - 255;
  syslog (LOG_DEBUG, "Command '...%s' ready\n", command + offset);

  setenv ("USER", env->user, 1);
  setenv ("AGENT_ADDRESS", env->agent_address, 1);
  setenv ("PATH",
	  "/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/opendomo",
	  1);

  /// @todo use secure popen
  FILE *f = popen (command, "r");
  if(!f)
  {
    syslog (LOG_ERR, "%s (%d): %s\n", strerror (errno), errno, cmd);
    return -1;
  }
  else
  {
    syslog (LOG_DEBUG, "Executing '%s'\n", command);
  }

  // get script name (without extension)
  sstrncpy (script_name, cmd, sizeof (script_name));
  char *p = strstr (script_name, " ");
  if(p)
    *p = 0;

  sstrncpy (script_name, basename (script_name), sizeof (script_name));
  sstrncpy (cmdid, script_name, sizeof (cmdid));

  p = strstr (cmdid, ".");
  if(p)
    *p = 0;

	if(gui == html)
	printf ("<div id='%s'>\n", section);
	enum options
	{
		none = 0,
		list = 1,
		form = 2,
		actions = 3,
		categories = 4,
		text = 5,
		graph = 6
	} opt;
	opt = none;

//   const int arraylen = 5;
//   char *array[arraylen];
//   string_vector_t *vs = string_vector_alloc();

  /* parse command output */
  while(fgets (buf, sizeof (buf), f) != NULL)
  {
    /* New format proposed:
       The new format will use the first character of a line to determine the
       whole structure, reducing the amount of regular expressions and string
       comparing functions called.  With only this character and the context, 
       assuming "context" as the last container element opened, we can define
       how to proceed.
       FC Cont.  Processed as ...
       #  any HTML comment, hidden from the rendering
       \t form   LI element, form field
       \t list   LI element, list (chart) item
       \t action Button in the toolbar, affecting all the items selected
       \t tabs   New tab element
       \n any End of opened section

       In the other cases, the string will be compared with some fixed-lenght
       strings to determine if it's a section opening or a warning/error/text
       paragraph, which is the default case.
     */

	// ========== Long text block {} =====================
	if(opt == text)
	{
		if(buf[0] == '}')
		{
			if(gui == html)
			{
	  			printf ("</textarea>\n");
			}
			else
			{
				printf ("</text>\n");
			}
			opt = form;
		}
		else
			printf ("%s", buf);
	}
	// ================ "#" Text block ===================
	else if(buf[0] == '#')
	{
		script_process_comments (buf + 1);
	}

	// ===== Empty line (closing of previous section) ====
	else if(buf[0] == '\n')
	{
		switch (opt)
		{
		case categories:
			script_process_categories_footer ();
			break;
		case form:
			script_process_form_footer (script_name);
			break;
		case graph:
			script_process_graph_footer (script_name, TRUE);
			break;
		case actions:
			script_process_action_footer (cmdid);
			break;
		case list:
			script_process_list_footer ();
			break;
		case none:
		case text:
			fflush (stdout);
			break;
		}
		opt = none;
	}
	// ===== Tab in first character (listing elements) ===
	else if(buf[0] == '\t')
	{
		lsid = strtok (buf, "\t\n");
		lsname = strtok (NULL, "\t\n");
		lstype = strtok (NULL, "\t\n");
		lsextra = strtok (NULL, "\t\n");
		//printf("\n<!-- strtok test:\n\t %s\n\t- %s \n\t- %s \n\t- %s -->\n",
		// lsid,lsname,lstype,lsextra);
		// ========= SECTION BODIES ============================
		switch (opt)
		{
		case form:		// Body "form" section
			if(lstype == NULL)
				continue;		// Avoiding white-screen-of-death 
			if(gui == html)
				script_process_form_body (lsid, lsname, lstype, lsextra);
			else
		  		script_process_form_body_xml (lsid, lsname, lstype, lsextra);
			break;
		case actions:		// Body "actions" section
			script_process_action_body (lsid, lsname, lstype);
			submit2set = 0;
			break;
		case list:		// Body "list" section,
			if(lstype == NULL)
				continue;		// Avoiding white-screen-of-death 
			script_process_list_body (lsid, lsname, lstype, lsextra);
			break;
		case graph:		// Graph plot lines     
			script_process_graph_body (lsid, lsname, lstype, lsextra);
			break;
		case categories:		// Categories
			script_process_categories_body (lsid, lsname, lstype, lsextra);
			break;
		default:
			printf ("\n<!-- unknown section '%s' in %d -->\n", buf, opt);
			break;
		}
	}
	// ========== SECTION HEADERS ========================  
	// Manual page of this section: http://www.opendomo.com/script-section   
	// "categories" Section
	else if(SPELL4 (buf, 't', 'a', 'b', 's'))
	{
		opt = categories;

		script_process_categories_header ();
	}

	// "form" Section
	else if(SPELL4 (buf, 'f', 'o', 'r', 'm'))
	{
		lsid = strtok (buf, ":\n");
		lsname = strtok (NULL, "\t\n");
		if(lsname[0] == ' ')
			script_format_error (script_name,
				"Unexpected space character after ':'");
		sstrncpy (script_name, lsname, sizeof (script_name));
		lstype = strtok (NULL, "\t\n");
		lsid = strtok (lsname, ".\n");
		sstrncpy (cmdid, lsid, sizeof (cmdid));
		// **********************************************************
		opt = form;
		if(usetabs == 0)
		{
			usetabs = 1;
			if(gui == html)
				printf ("<div class='tabber'>\n");
		}
		script_process_form_header (lsid, legend, lstype);
	}

	// "graph" Section
	else if(SPELL4 (buf, 'g', 'r', 'a', 'p'))
	{
		opt = graph;
		lsid = strtok (buf, ":\n");
		lsname = strtok (NULL, "\t\n");
		if(lsname[0] == ' ')
			script_format_error (script_name,
				"Unexpected space character after ':'");
		sstrncpy (script_name, lsname, sizeof (script_name));
		lstype = strtok (NULL, "\t\n");
		lsid = strtok (lsname, ".\n");

		script_process_graph_header (lsid, legend, lstype);
    }

    // "list" Section
    else if(SPELL4 (buf, 'l', 'i', 's', 't'))
    {
		// *** Moure tot això al principi del bloc ******************
		char listboxtype[] = "listbox";
		lsid = strtok (buf, ":\n");	// jumping the prefix "list:"
		lsname = strtok (NULL, "\t\n");
		if(lsname[0] == ' ')
			script_format_error (script_name,
				"Unexpected space character after ':'");
		sstrncpy (script_name, lsname, sizeof (script_name));
		lstype = strtok (NULL, "\t\n");
		lsid = strtok (lsname, ".\n");
		sstrncpy (cmdid, lsid, sizeof (cmdid));
		// **********************************************************
		opt = list;
		if(lsname == NULL)
		{
			script_format_error (script_name, "Script name not specified");
		}
		if(lstype == NULL)
			lstype = listboxtype;

		if(usetabs == 0 && strcmp (lstype, "simple") != 0)
		{
			usetabs = 1;
			if(gui == html)
			printf ("<div class='tabber'>\n");
		}

		if(strcmp (lstype, "simple") == 0)
			lstype = listboxtype;

		script_process_list_header (lsid, legend, lstype);
    }

	// "actions" Section
	else if(SPELL4 (buf, 'a', 'c', 't', 'i'))
	{
		if((opt == list) || (opt == form))
		{
			if(gui == html)
			{
				printf ("\n\t\t\t</ul>\n");
			}
			else
			{
				printf ("\n\t</gui>\n");
			}
		} else if (opt == graph) {
			script_process_graph_footer (script_name, FALSE);
		}
		opt = actions;
		script_process_action_header (cmdid);
    }

    else
    {
      printf ("\n<!-- Non-formated line '%s' -->\n", buf);
      script_format_error (script_name, "Not formatted line");
    }

  }

  int retval = pclose (f);
  if(gui == html)
  {
    if(retval == 0)
    {
      // Everything was fine
      if(opt != none)
	script_format_error (script_name, "Section not closed. Missing CR");
    }
    else if(retval == -1)
    {
      // Some problem happened when closing the pipe
      printf ("<p class='error'>%s (%d): %s</p>\n"
              "<div id='tbrerror' class='toolbar'>\n",
	      strerror (errno), errno, cmd);
      printf (" <button onclick='history.back();'>%s</button>",
	      T (ODCGI_TEXT_BACK));
      printf ("</div>\n");
      syslog (LOG_ERR, "%s (%d): %s\n", strerror (errno), errno, cmd);
    }
    else
    {
      // Error code returned by the script 
      printf ("\t\t<p class='error'>%s</p>\n"
        "<div id='tbrerror' class='toolbar'>",
	      T (ODCGI_TEXT_ERROR_HELP));
      printf (" <button onclick='history.back();'>%s</button>",
	      T (ODCGI_TEXT_BACK));
      printf (" <button  "
	      "onclick=\"showBubble('%s%s#error%d');\">%s</button>",
	      ODCGI_URL_HELP, cmdid, WEXITSTATUS (retval),
	      T (ODCGI_TEXT_ERROR_HELP_LINK));
      printf ("</div>\n");

      syslog (LOG_ERR, "return (%d): %s\n", WEXITSTATUS (retval), cmd);
    }

    if(usetabs && gui == html)
      printf ("</div>\n");

    printf ("</div>\n");
    //fflush(stdout);
  }

  //fflush(stdout);
  return 0;
}

// }}}

// {{{
int
parse_list (const char *input, const char *values)
{
	char buffer[10000];
	char sbuf1[10000];
	char sbuf2[10000];
	char grp = FALSE;
	char *closing;

	str_assign_str (buffer, sizeof (buffer), input);
	closing = strchr (buffer, ']');
	if(closing == NULL)
	{
		// No list ending? Abort.
		return -1;
	}
	buffer[closing - buffer] = 0;

	if(!str_starts_with (buffer, "list["))
	{
		return -1;
	}

	//printf("<!-- parsing_list [%s] -->\n", buffer);

	str_replace (buffer, sizeof (buffer), "list[", "");
	str_replace (buffer, sizeof (buffer), "]", "");

	// Notes: the supported list formats are
	//  - list[a,b,c,d]
	// - list[a:A,b:B,c:C,d:D] (being the uppercase understood as the literal)
	// To open an option group, we will put a "@" character at the beginning of
	// an item that won't be selectable. The following items will belong to this
	// group until another group is opened.   

	char *str1 = buffer;
	// One element per iteration
	while((str1 = str_split (str1, ",", sbuf1, sizeof (sbuf1))))
	{
	//	printf("<!-- \n\tstr1: %s \n\tsbuf1: %s \n\tsbuf2: %s -->\n",
	//		str1, sbuf1, sbuf2);
    		char *str2 = sbuf1;
    		if(str2[0] != '@')
		{
			printf ("\t<option");
			if((str2 = str_split (str2, ":", sbuf2, sizeof (sbuf2))))
			{
				str_trim (sbuf2);
				if(0 == strcmp (sbuf2, values)) {
					printf (" selected='selected'");
			//	} else {
			//		printf (" selected='unselected'");
				}
				printf (" value='%s'", sbuf2);
				str2 = str_split (str2, ":", sbuf2, sizeof (sbuf2));
			} else {
				if (0 == strcmp(sbuf2,values)) {
					printf (" selected='selected'");
			//	} else {
			//		printf (" selected='unselected'");
				}
			}
			str_trim (sbuf2);
			printf (">%s</option>\n", CT (sbuf2));

		} else {
			if(grp == TRUE)
				printf ("</optgroup>\n");
			printf ("<optgroup id='%s' ", ++str2);
			while((str2 = str_split (str2, ":", sbuf2, sizeof (sbuf2))))
			{
				str_trim (sbuf2);
				printf (" value='%s' ", sbuf2);
			}
			str_trim (sbuf2);
			printf ("label=\"%s\">\n", CT (sbuf2));
			grp = TRUE;
		}
	}

	char *str2 = sbuf1;
	if(str2[0] != '@')
	{
		printf ("\t<option");
		if((str2 = str_split (str2, ":", sbuf2, sizeof (sbuf2))))
		{
			str_trim (sbuf2);
			//printf("<!-- comparing %s - %s - %s -->\n", str2, sbuf2, values);
			if(0 == strcmp (sbuf2, values)) {
				printf (" selected='selected'");
		//	} else { 
		//		printf (" selected='unselected'");
			}
			printf (" value='%s'", sbuf2);
			str2 = str_split (str2, ":", sbuf2, sizeof (sbuf2));
		} else {
			if(0 == strcmp (sbuf2, values)) {
				printf (" selected='selected'");
		//	} else { 
		//		printf (" selected='unselected'");
			}		
		}
		str_trim (sbuf2);
		printf (">%s</option>\n", CT (sbuf2));

	} else {
		if(grp == TRUE)
			printf ("</optgroup>\n");
		printf ("<optgroup id='%s' ", ++str2);
		while((str2 = str_split (str2, ":", sbuf2, sizeof (sbuf2))))
		{
			str_trim (sbuf2);
			printf (" value='%s'", sbuf2);
		}
		str_trim (sbuf2);
		printf ("label=\"%s\">\n", CT (sbuf2));
		grp = TRUE;
	}

	if(grp == TRUE)
		printf ("</optgroup>\n");
	return 0;
}

// }}}

