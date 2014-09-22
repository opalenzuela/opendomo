#!/usr/bin/env python
import cgi, os, sys
import cgitb; cgitb.enable()

try: # Windows needs stdio set for binary mode.
    import msvcrt
    msvcrt.setmode (0, os.O_BINARY) # stdin  = 0
    msvcrt.setmode (1, os.O_BINARY) # stdout = 1
except ImportError:
    pass

# This directory is only needed for this particular feature. 
# So, if it does not exists, we create it.
if not os.path.exists("/var/opendomo/tmp/lighttpd/upload"):
    os.makedirs("/var/opendomo/tmp/lighttpd/upload")

form = cgi.FieldStorage()

if "fname" not in form:
	message = 'Invalid format'
	print """\
	Content-Type: text/html\n
	<html><head><link rel='stylesheet' type='text/css' href='/css/fileupload.css' /></head>
	<body>
	<h2>%s</h2>
	</body></html>
	""" % (message,)
	sys.exit()
else:
	filename = form.getvalue("fname")
	try: 
		fn = '/var/opendomo/tmp/' + os.path.basename(filename)
		f = open(fn, 'r')
		overwrite = 'Overwrite'
		overwriteclass = 'fileexists'

	except IOError:
		overwrite = 'Send file'
		overwriteclass = 'newfile'

	
if "file" not in form:
	print """\
	Content-Type: text/html\n
	<html><head><link rel='stylesheet' type='text/css' href='/css/fileupload.css' /></head>
	<body>
	<form enctype='multipart/form-data' action='fileupload.py' method='post' class='%s'>
	<label><input name='file' type='file'><div class='filecontainer'>%s</div></labe>
	<input name='fname' type='hidden' value='%s'>
	<input type='submit'></p>
	</body></html>
	""" % (overwriteclass,overwrite,filename)
	sys.exit()

# Generator to buffer file chunks
def fbuffer(f, chunk_size=10000):
   while True:
      chunk = f.read(chunk_size)
      if not chunk: break
      yield chunk
      
# A nested FieldStorage instance holds the file
fileitem = form['file']

# Test if the file was uploaded
if fileitem.filename:

   # strip leading path from file name to avoid directory traversal attacks
   fn = '/var/opendomo/tmp/' + os.path.basename(filename)
   
   f = open(fn, 'wb', 10000)

   # Read the file in chunks
   for chunk in fbuffer(fileitem.file):
      f.write(chunk)
   f.close()
   message = 'File uploaded successfully'

else:
   message = 'No file was uploaded'
   
print """\
Content-Type: text/html\n
<html><head><link rel='stylesheet' type='text/css' href='/css/fileupload.css' /></head>
<body>
<form><div class='filecontainer'>%s</div></form>
</body></html>
""" % (message,)