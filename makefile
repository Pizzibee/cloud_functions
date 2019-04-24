CFLAGS = -D_GNU_SOURCE -std=c11 -pedantic -Wall -Wvla -Werror

SERVER_PATH = server/
CLIENT_PATH = client/
TYPE_PATH = types/
UTILS_PATH = utils/

server: $(SERVER_PATH)server.c
	cc $(CFLAGS) -o $(SERVER_PATH)serverTest $(SERVER_PATH)server.c




clear:
	clear

clean:
	rm -f *.o
	rm -f $(SERVER_PATH)serverTest
