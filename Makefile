# Compiler
CC=gcc
# Flags
CFLAGS=-Wall -lpthread -lm -O3
# OBJECTS
BUILD:=build
SRCDIR:=src
MATRIX:=matrix
WORKERS:=workers
CODEC:=codec
FUNCTIONS:=functions
MAIN:=main

codec:
	@echo "Building $@..."
	@$(CC) -O3 -o $(BUILD)/$(MATRIX).o -c $(SRCDIR)/$(MATRIX).c
	@$(CC) -O3 -o $(BUILD)/$(WORKERS).o -c $(SRCDIR)/$(WORKERS).c
	@$(CC) -O3 -o $(BUILD)/$(CODEC).o -c $(SRCDIR)/$(CODEC).c
	@$(CC) -O3 -o $(BUILD)/$(FUNCTIONS).o -c $(SRCDIR)/$(FUNCTIONS).c
	@$(CC) -O3 -o $(BUILD)/$(MAIN).o -c $(SRCDIR)/$(MAIN).c

	@$(CC) -o $@ \
		$(BUILD)/$(MATRIX).o \
		$(BUILD)/$(WORKERS).o \
		$(BUILD)/$(CODEC).o \
		$(BUILD)/$(FUNCTIONS).o \
		$(BUILD)/$(MAIN).o \
		$(CFLAGS)
	@echo " › $@ built"

bench: codec
	@./bench.sh -f 64 -s
	@./bench.sh -f 512 -s
	@./bench.sh -f 1024

# Clean
clean:
	@rm -f build/original
	@rm -f build/encoded
	@rm -f build/decoded
	@rm -f build/codec
	@rm -f codec
