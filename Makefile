OBJ_MAIN = src/main.c
HEADERS = $(wildcard src/*.h)
OBJECTS = $(patsubst src/%.c, bin/%.o, $(wildcard src/*.c))
TESTS = test/testlist.c

#compiler we're using
CC = gcc

#additional compilation options
CFLAGS = -std=c99 -pedantic -Wall -Wextra

#libraries we're linking against
LIBS = -lSDL2 -lSDL2_image -lm

#these are only used for debug
DEBUG_FLAGS = -pg

#specifies the name of our executable
TARGET = shipsdl

default: $(TARGET)
all: default

bin/%.o: src/%.c $(HEADERS)
	$(CC) $(CFLAGS) $(LIBS) $(DEBUG_FLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) $(LIBS) $(DEBUG_FLAGS) -o $@

dependency_graph: $(OBJECTS) $(HEADERS)
	PWD="$PWD/src" util/cinclude2dot.pl | dot -Teps > dep_graph.eps

clean:
	rm -f bin/*.o
	rm -f $(TARGET)
	rm -f dep_graph.eps
