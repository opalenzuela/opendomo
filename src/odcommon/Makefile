FLAGS = -Wall -g 
HDRS = -I .
LIB_COELACANTH=usr/lib/libcoelacanth.so
SRC_COELACANTH=src/string.c src/str.c

default:
	$(CC) $(FLAGS) $(HDRS) -fPIC -shared -o $(LIB_COELACANTH) $(SRC_COELACANTH) -I. 
	$(CC) $(FLAGS) $(HDRS) src/bgshell.c -o usr/local/bin/bgshell

clean:
	rm -f usr/local/bin/bgshell
	rm -f usr/lib/libcoelacanth.so
