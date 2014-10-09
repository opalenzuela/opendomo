OPTS= -Wall -std=gnu99 -pedantic -O2 -s
HDRS= -I /usr/include -I src/ -I ../odcommon/
LIBS= -lssl -lcrypto -lcrypt -lconfig -ljpeg -lm

# -- default ------------------------------------------------------------------
default: odcgi

# -- odcgi --------------------------------------------------------------------
ODCM_FILES = ../odcommon/src/str.c ../odcommon/src/string.c ../odcommon/src/match.c ../odcommon/src/base64.c ../odcommon/src/jpeg.c  ../odcommon/src/image.c ../odcommon/src/mjpg.c
ODCGI_OBJS = src/mjpg.o src/i18n.o src/user.o src/file.o src/util.o src/session.o src/cgi.o src/secfunc.o src/script.o
                
odcgi: $(ODCGI_OBJS) 
	$(CC) $(OPTS) $(HDRS)  $(ODCM_FILES) $(ODCGI_OBJS) $(LIBCONFIG_FILES) src/odcgi.c -o var/www/cgi-bin/od.cgi -g $(LIBS)
	#$(CC) $(OPTS) $(HDRS) src/secfunc.o src/user.o  src/test.c -o bin/test -l crypt

# -- standard object ----------------------------------------------------------
%.o: %.c %.h
	$(CC) $(OPTS) $(HDRS) -c $< -o $@

clean:
	rm -f */*.o 
