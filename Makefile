OBJ_MAIN = src/main.c
OBJS = src/LTexture.h src/LTexture.c src/GameObject.c src/GameObject.h \
	src/Graphics.h src/Graphics.c src/List.c src/List.h src/Real2.c \
	src/Real2.h src/Assets.c src/Assets.h src/Typedefs.h src/Physics.c \
	src/Physics.h src/Collider.c src/Collider.h src/Polygon.c src/Polygon.h
TESTS = test/testlist.c

#compiler we're using
CC = gcc

#additional compilation options
COMPILER_FLAGS = -w

#libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_image -lm

#these are only used for debug
DEBUG_FLAGS = -pg

#specifies the name of our executable
OBJ_NAME = shipsdl

all : $(OBJ_MAIN) $(OBJS)
	$(CC) $(OBJ_MAIN) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

debug : $(OBJS)
	$(CC) $(OBJ_MAIN) $(OBJS) $(COMPILER_FLAGS) $(DEBUG_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

tests : $(TESTS)
	./compile_tests.sh $(OBJS) $(LINKER_FLAGS) $(COMPILER_FLAGS)

.PHONY : test
test :
	./run_tests.sh

.PHONY : clean
clean :
	rm $(OBJ_NAME)

