# DOT makefile
# also, don't change the first character of the file ('#'), as it's used for file tests.

FLAGS=-Wall -Wextra -g
CFLAGS=$(shell pkg-config --cflags glfw3)
LDFLAGS=$(shell pkg-config --libs glfw3) -lGL -lm -lGLEW -lGLU

CC=gcc

OBJS=util.o block.o program.o log.o file.o config.o window.o shader.o object.o world.o main.o

down:
	cd ../ && make src

src: $(OBJS)
	$(CC) -o $(NAME) $(OBJS) $(FLAGS) $(LDFLAGS)

%.o: %.c ../include/
	$(CC) -c -o $@ $< $(FLAGS) $(CFLAGS) -I../include/

.PHONY: all clean

clean:
	-rm *~
	-rm $(NAME)
	-rm *.o
