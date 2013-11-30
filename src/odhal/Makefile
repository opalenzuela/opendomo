OPTS= -Wall
HDRS= -I $(TOOLCHAIN)/usr/include -I ctrlpt/ -I device/ -I common/
LIBS= -I ../libcoelacanth/ ../libcoelacanth/coelacanth/str.c



# -- default -----------------------------------------------------------------
default: 
	$(CC) $(OPTS) $(HDRS) $(LIBS) src/printlog.c src/readline.c src/semaphore.c src/rmfr.c src/fifo.c src/domino.c -o usr/bin/domino
	$(CC) $(OPTS) $(HDRS) $(LIBS) src/printlog.c src/readline.c src/rmfr.c src/fifo.c src/dummy.c -o usr/bin/dummy
	$(CC) $(OPTS) $(HDRS) $(LIBS) src/readline.c src/printlog.c src/semaphore.c src/rmfr.c src/fifo.c src/micropik.c -o usr/bin/micropik
	$(CC) $(OPTS) $(HDRS) $(LIBS) src/printlog.c src/readline.c src/semaphore.c src/rmfr.c src/fifo.c src/arduino.c -o usr/bin/arduino
	$(CC) $(OPTS) $(HDRS) $(LIBS) -lusb src/printlog.c src/readline.c src/rmfr.c src/fifo.c src/x10lib.c src/x10.c -o usr/bin/x10 -lusb




clean:
	rm -f usr/bin/*
	rm -f */*.o 
	rm -fr pkg/o*


