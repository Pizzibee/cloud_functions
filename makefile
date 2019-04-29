CFLAGS = -D_GNU_SOURCE -std=c11 -pedantic -Wall -Wvla -Werror

SERVER_PATH = server/
CLIENT_PATH = client/
TYPE_PATH = types/
UTILS_PATH = utils/
MAINTANENANCE_PATH = server/maintenance/

all:server maint

server: $(SERVER_PATH)server.o $(MAINTANENANCE_PATH)maintenanceModule.o $(UTILS_PATH)check.o
	cc $(CFLAGS) -g -o serverTest $(SERVER_PATH)server.o $(MAINTANENANCE_PATH)maintenanceModule.o $(UTILS_PATH)check.o

server.o: $(SERVER_PATH)server.c $(MAINTANENANCE_PATH)maintenanceModule.h  $(TYPE_PATH)types.h
	cc $(CFLAGS) -c  $(SERVER_PATH)server.c

maint: $(MAINTANENANCE_PATH)maint.o $(MAINTANENANCE_PATH)maintenanceModule.o $(UTILS_PATH)check.o $(TYPE_PATH)types.h
	cc $(CFLAGS) -o maint $(MAINTANENANCE_PATH)maint.o $(MAINTANENANCE_PATH)maintenanceModule.o $(UTILS_PATH)check.o

maint.o: $(MAINTANENANCE_PATH)maint.c $(MAINTANENANCE_PATH)maintenanceModule.h  $(TYPE_PATH)types.h
	cc $(CFLAGS) -c $(MAINTANENANCE_PATH)maint.c

maintenanceModule.o: $(MAINTANENANCE_PATH)maintenanceModule.c $(MAINTANENANCE_PATH)maintenanceModule.h  $(TYPE_PATH)types.h
	cc $(CFLAGS) -c $(MAINTANENANCE_PATH)maintenanceModule.c

utils.o: $(UTILS_PATH)utils.h $(UTILS_PATH)utils.c
	cc $(CFLAGS) -c $(UTILS_PATH)utils.c

check.o: $(UTILS_PATH)check.h $(UTILS_PATH)check.c
	cc $(CFLAGS) -c $(UTILS_PATH)check.c

clear:
	clear

clean:
	rm -f *.o
	rm -f $(SERVER_PATH)serverTest
