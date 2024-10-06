.PHONY: all thread address compile compile_debug_thread compile_debug_address

all: compile run

thread: compile_debug_thread run

address: compile_debug_address run

compile:
	gcc src/* -o bot -lssl -lcrypto -ljansson

compile_debug_thread:
	gcc -fdiagnostics-color=always -g -fsanitize=thread -O0 -DLOCALENV=0 src/* -o bot -lssl -lcrypto -ljansson

compile_debug_address:
	gcc -fdiagnostics-color=always -g -fsanitize=address -O0 -DLOCALENV=0 src/* -o bot -lssl -lcrypto -ljansson

run:
	./bot