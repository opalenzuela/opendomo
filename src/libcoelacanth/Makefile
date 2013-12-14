FLAGS = -Wall -g 
HDRS = -I .
LIB_COELACANTH=usr/lib/libcoelacanth.so
SRC_COELACANTH=coelacanth/string.c coelacanth/str.c
LLIB=-lcoelacanth -L usr/lib/

default:
	$(CC) $(FLAGS) $(HDRS) -fPIC -shared -o $(LIB_COELACANTH) $(SRC_COELACANTH) -I. 
	$(CC) $(FLAGS) $(HDRS) src/str.c -o bin/str $(LLIB)
	$(CC) $(FLAGS) $(HDRS) src/string.c -o bin/string $(LLIB)
	$(CC) $(FLAGS) $(HDRS) src/vector.c -o bin/vector
	$(CC) $(FLAGS) $(HDRS) src/stack.c -o bin/stack
	$(CC) $(FLAGS) $(HDRS) src/queue.c -o bin/queue
	$(CC) $(FLAGS) $(HDRS) src/hashtable.c -o bin/hashtable
	$(CC) $(FLAGS) $(HDRS) src/reboot.c -o usr/bin/reboot_u
	$(CC) $(FLAGS) $(HDRS) unix/user.c src/usermm.c -o usr/bin/usermm -lcrypt $(LLIB)
	$(CC) $(FLAGS) $(HDRS) src/bgshell.c -o usr/bin/bgshell
	$(CC) $(FLAGS) $(HDRS) coelacanth/str.c src/sqlite-client.c -o usr/bin/sqlite-client -lsqlite3 

clean:
	rm -f bin/*
	rm -f usr/bin/*
	rm -f usr/lib/*
