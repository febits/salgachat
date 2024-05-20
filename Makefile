CC=gcc
CFLAGS=-Wall -Wextra -pedantic -Iinclude

SRC_CLIENT=$(wildcard src/client/*.c)
SRC_CLIENT+=$(wildcard src/core/*.c)

SRV_CLIENT=$(wildcard src/server/*.c)
SRV_CLIENT+=$(wildcard src/core/*.c)

CLIENT=build/client
SERVER=build/server

.PHONY: all clean always

all: $(CLIENT) $(SERVER)

$(CLIENT): $(SRC_CLIENT) always
	$(CC) $(CFLAGS) $< -o $@

$(SERVER): $(SRC_CLIENT) always
	$(CC) $(CFLAGS) $< -o $@

always:
	mkdir -p build

clean:
	rm -rf build
