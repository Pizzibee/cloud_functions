CFLAGS = -D_GNU_SOURCE -std=c11 -pedantic -Wall -Wvla -Werror

SERVER_PATH = server/
CLIENT_PATH = client/
TYPE_PATH = types/
MODULES_PATH = modules/

all:server maint gstat client

server: $(SERVER_PATH)server.o $(MODULES_PATH)sharedMem.o $(MODULES_PATH)check.o $(MODULES_PATH)utils.o $(MODULES_PATH)socket.o
	cc $(CFLAGS) -g -o serverTest $(SERVER_PATH)server.o $(MODULES_PATH)sharedMem.o $(MODULES_PATH)check.o $(MODULES_PATH)utils.o $(MODULES_PATH)socket.o

maint: $(SERVER_PATH)maint.o $(MODULES_PATH)sharedMem.o $(MODULES_PATH)check.o $(TYPE_PATH)types.h
		cc $(CFLAGS) -o maint $(SERVER_PATH)maint.o $(MODULES_PATH)sharedMem.o $(MODULES_PATH)check.o

gstat: $(SERVER_PATH)gstat.o $(MODULES_PATH)sharedMem.o  $(TYPE_PATH)types.h
		cc $(CFLAGS) -o gstat $(SERVER_PATH)gstat.o $(MODULES_PATH)sharedMem.o $(MODULES_PATH)check.o

client: $(CLIENT_PATH)client.o $(MODULES_PATH)utils.o $(MODULES_PATH)check.o $(MODULES_PATH)socket.o
	cc $(CFLAGS) -o clientTest $(CLIENT_PATH)client.o $(MODULES_PATH)utils.o $(MODULES_PATH)check.o $(MODULES_PATH)socket.o

client.o: $(CLIENT_PATH)client.c $(MODULES_PATH)utils.h
	cc $(CFLAGS) -c $(CLIENT_PATH)client.c

gstat.o: $(SERVER_PATH)gstat.c $(MODULES_PATH)sharedMem.h  $(TYPE_PATH)types.h
	cc $(CFLAGS) -c  $(SERVER_PATH)gstat.c

server.o: $(SERVER_PATH)server.c $(MODULES_PATH)sharedMem.h  $(TYPE_PATH)types.h
	cc $(CFLAGS) -c  $(SERVER_PATH)server.c

maint.o: $(SERVER_PATH)maint.c $(MODULES_PATH)sharedMem.h  $(TYPE_PATH)types.h
	cc $(CFLAGS) -c $(SERVER_PATH)maint.c

sharedMem.o: $(MODULES_PATH)sharedMem.c $(MODULES_PATH)sharedMem.h  $(TYPE_PATH)types.h
	cc $(CFLAGS) -c $(MODULES_PATH)sharedMem.c

utils.o: $(MODULES_PATH)utils.h $(MODULES_PATH)utils.c $(MODULES_PATH)check.h
	cc $(CFLAGS) -c $(MODULES_PATH)utils.c

check.o: $(MODULES_PATH)check.h $(MODULES_PATH)check.c
	cc $(CFLAGS) -c $(MODULES_PATH)check.c

socket.o: $(MODULES_PATH)socket.h $(MODULES_PATH)socket.c
	cc $(CFLAGS) -c $(MODULES_PATH)socket.c

clear:
	clear

clean:
	rm -f $(SERVER_PATH)*.o
	rm -f $(MODULES_PATH)*.o
	rm -f $(CLIENT_PATH)*.o
	rm -f serverTest
	rm -f maint
	rm -f gstat
	rm -f clientTest
