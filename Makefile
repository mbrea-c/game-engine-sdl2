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

clean:
	rm bin/*.o
	rm $(TARGET)

#debug : $(OBJS)
	#$(CC) $(OBJ_MAIN) $(OBJS) $(COMPILER_FLAGS) $(DEBUG_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

#tests : $(TESTS)
	#./compile_tests.sh $(OBJS) $(LINKER_FLAGS) $(COMPILER_FLAGS)

#.PHONY : test
#test :
	#./run_tests.sh

#.PHONY : clean
#clean :
	#rm $(OBJ_NAME)

