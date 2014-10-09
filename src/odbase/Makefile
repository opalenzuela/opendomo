OPTS= -Wall -std=gnu99 -pedantic -O2 -s
HDRS= -I /usr/include -I src/ 
LIBS= -lssl -lcrypto -lcrypt -lconfig -ljpeg -lm


# -- odcgi --------------------------------------------------------------------
ODCM_FILES = src/str.c src/string.c src/match.c .src/base64.c src/jpeg.c  src/image.c src/mjpg.c
ODCGI_OBJS = src/mjpg.o src/i18n.o src/user.o src/file.o src/util.o src/session.o src/cgi.o src/secfunc.o src/script.o
                
default:  
	$(CC) $(OPTS) $(HDRS)  $(ODCM_FILES) $(ODCGI_OBJS) $(LIBCONFIG_FILES) src/odcgi.c -o var/www/cgi-bin/od.cgi -g $(LIBS)
	#$(CC) $(OPTS) $(HDRS) src/secfunc.o src/user.o  src/test.c -o bin/test -l crypt
	$(CC) $(FLAGS) $(HDRS) src/bgshell.c -o usr/local/bin/bgshell

# -- standard object ----------------------------------------------------------
%.o: %.c %.h
	$(CC) $(OPTS) $(HDRS) -c $< -o $@

clean:
	rm -f */*.o 
	rm -f usr/local/bin/bgshell
