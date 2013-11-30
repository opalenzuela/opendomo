OPTS= -Wall -std=gnu99 -pedantic -O2 -s
HDRS= -I $(TOOLCHAIN)/usr/include -I src/ -I ../libcoelacanth -I ../opendomo-ai/src/  #-I libconfig/ 
LIBS= -lssl -lcrypto -lcrypt -lconfig -ljpeg -lm




# -- default ------------------------------------------------------------------
default: odcgi


# -- libconfig ----------------------------------------------------------------
#LIBCONFIG_FILES = libconfig/grammar.c libconfig/scanner.c libconfig/libconfig.c


# -- odcgi --------------------------------------------------------------------
ODCM_FILES = ../libcoelacanth/coelacanth/str.c ../libcoelacanth/coelacanth/string.c ../libcoelacanth/regex/match.c ../libcoelacanth/crypto/base64.c ../libcoelacanth/ai/perceiving/vision/jpeg.c  ../libcoelacanth/ai/perceiving/vision/image.c ../libcoelacanth/ai/perceiving/vision/mjpg.c
ODCGI_OBJS = src/mjpg.o src/i18n.o src/user.o src/file.o src/util.o src/session.o src/cgi.o src/secfunc.o src/script.o
                
odcgi: $(ODCGI_OBJS) 
	$(CC) $(OPTS) $(HDRS)  $(ODCM_FILES) $(ODCGI_OBJS) $(LIBCONFIG_FILES) src/odcgi.c -o var/www/cgi-bin/od.cgi -g $(LIBS)
	#$(CC) $(OPTS) $(HDRS) src/secfunc.o src/user.o  src/test.c -o bin/test -l crypt



# -- standard object ----------------------------------------------------------
%.o: %.c %.h
	$(CC) $(OPTS) $(HDRS) -c $< -o $@

clean:
	rm -f */*.o 




