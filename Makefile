# Compiler
CC=gcc
# Flags
CFLAGS=-Wall -lpthread -lm -O2
# OBJECTS
OBJECTS=main.c

## TARGETS
# codec
codec :
	$(CC) $(CFLAGS) -o codec $(OBJECTS)
# Clean
clean : 
	rm codec
