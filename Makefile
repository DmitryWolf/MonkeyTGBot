.PHONY: all compile

all: compile

compile:
	gcc src/* -o /usr/local/bin/monkeybot -lssl -lcrypto -ljansson