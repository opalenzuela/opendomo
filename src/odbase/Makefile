OPTS= -Wall -std=gnu99 -pedantic -O2 -s
HDRS= -I /usr/include -I src/ -I.
LIBS= -lssl -lcrypto -lcrypt -lconfig -lm
#-ljpeg

# -- default ------------------------------------------------------------------
default: odcgi

# -- odcgi --------------------------------------------------------------------
ODCM_FILES = src/str.c src/string.c src/match.c src/base64.c 
# src/jpeg.c  src/image.c src/mjpg.c src/odcgi_mjpg.c
ODCGI_OBJS = src/i18n.o src/user.o src/file.o src/util.o src/session.o src/cgi.o src/secfunc.o src/script.o
#src/mjpg.o 
                
odcgi: $(ODCGI_OBJS) 
	$(CC) $(OPTS) $(HDRS)  $(ODCM_FILES) $(ODCGI_OBJS) $(LIBCONFIG_FILES) src/odcgi.c -o var/www/cgi-bin/od.cgi -g $(LIBS)
	$(CC) $(FLAGS) $(HDRS) src/bgshell.c -o usr/bin/bgshell
	#$(CC) $(OPTS) $(HDRS) src/secfunc.o src/user.o  src/test.c -o bin/test -l crypt

# -- standard object ----------------------------------------------------------
%.o: %.c %.h
	$(CC) $(OPTS) $(HDRS) -c $< -o $@

clean:
	rm -f */*.o 
