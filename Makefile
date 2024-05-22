CC=gcc
CFLAGS=-Wall -Wextra -pedantic -Iinclude

SRC_CLIENT=$(wildcard src/client/*.c src/core/*.c)
SRC_SERVER=$(wildcard src/server/*.c src/core/*.c)

CLIENT=client-salga
SERVER=server-salga

.PHONY: all clean always

all: $(CLIENT) $(SERVER)

$(CLIENT): $(SRC_CLIENT)
	$(CC) $(CFLAGS) $^ -o $@

$(SERVER): $(SRC_SERVER)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(CLIENT) $(SERVER)
