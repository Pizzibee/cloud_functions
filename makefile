CFLAGS = -D_GNU_SOURCE -std=c11 -pedantic -Wall -Wvla -Werror

SERVER_PATH = server/
CLIENT_PATH = client/
TYPE_PATH = types/
UTILS_PATH = utils/

all:server maint gstat

server: $(SERVER_PATH)server.o $(UTILS_PATH)sharedMem.o $(UTILS_PATH)check.o
	cc $(CFLAGS) -g -o serverTest $(SERVER_PATH)server.o $(UTILS_PATH)sharedMem.o $(UTILS_PATH)check.o

maint: $(SERVER_PATH)maint.o $(UTILS_PATH)sharedMem.o $(UTILS_PATH)check.o $(TYPE_PATH)types.h
		cc $(CFLAGS) -o maint $(SERVER_PATH)maint.o $(UTILS_PATH)sharedMem.o $(UTILS_PATH)check.o

gstat: $(SERVER_PATH)gstat.o $(UTILS_PATH)sharedMem.o  $(TYPE_PATH)types.h
		cc $(CFLAGS) -o gstat $(SERVER_PATH)gstat.o $(UTILS_PATH)sharedMem.o $(UTILS_PATH)check.o

gstat.o: $(SERVER_PATH)gstat.c $(UTILS_PATH)sharedMem.h  $(TYPE_PATH)types.h
	cc $(CFLAGS) -c  $(SERVER_PATH)gstat.c

server.o: $(SERVER_PATH)server.c $(UTILS_PATH)sharedMem.h  $(TYPE_PATH)types.h
	cc $(CFLAGS) -c  $(SERVER_PATH)server.c

maint.o: $(SERVER_PATH)maint.c $(UTILS_PATH)sharedMem.h  $(TYPE_PATH)types.h
	cc $(CFLAGS) -c $(SERVER_PATH)maint.c

sharedMem.o: $(UTILS_PATH)sharedMem.c $(UTILS_PATH)sharedMem.h  $(TYPE_PATH)types.h
	cc $(CFLAGS) -c $(UTILS_PATH)sharedMem.c

utils.o: $(UTILS_PATH)utils.h $(UTILS_PATH)utils.c
	cc $(CFLAGS) -c $(UTILS_PATH)utils.c

check.o: $(UTILS_PATH)check.h $(UTILS_PATH)check.c
	cc $(CFLAGS) -c $(UTILS_PATH)check.c

clear:
	clear

clean:
	rm -f $(SERVER_PATH)*.o
	rm -f $(UTILS_PATH)*.o
	rm -f serverTest
	rm -f maint
	rm -f gstat
