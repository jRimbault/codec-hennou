# Compiler
CC=gcc
# Flags
CFLAGS=-Wall -lpthread -lm -O2
# OBJECTS
OBJECTS=src/main.c


codec :
	@echo "Building codec..."
	@$(CC) $(OBJECTS) $(CFLAGS) -o codec
	@echo " › Codec built"

bench : codec
	@./bench.sh -f 64 -s
	@./bench.sh -f 512 -s
	@./bench.sh -f 1024

# Clean
clean :
	@rm -f build/original
	@rm -f build/encoded
	@rm -f build/decoded
	@rm -f build/codec
	@rm -f codec

