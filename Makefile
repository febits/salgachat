CC=gcc
CFLAGS=-Wall -Wextra -pedantic -Iinclude

SRC_CLIENT=$(wildcard src/client/*.c src/core/*.c)
SRC_SERVER=$(wildcard src/server/*.c src/core/*.c)

CLIENT=build/client
SERVER=build/server

.PHONY: all clean always

all: always $(CLIENT) $(SERVER)

$(CLIENT): $(SRC_CLIENT)
	$(CC) $(CFLAGS) $^ -o $@

$(SERVER): $(SRC_SERVER)
	$(CC) $(CFLAGS) $^ -o $@

always:
	mkdir -p build

clean:
	rm -rf build
