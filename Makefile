CC = g++
CFLAGS = -Wall -g -c
LFLAGS = -lSDL -lSDL_image



all: obj/card.o obj/solitaire.o obj/table.o obj/window.o | bin/
	$(CC) $(LFLAGS) -o bin/solitaire $^

obj/%.o : src/%.cpp | obj/
	$(CC) $(CFLAGS) $< -o $@

obj/ :
	mkdir obj/

bin/ :
	mkdir bin/

.PHONY: clean
clean:
	rm -rf obj/
	rm -rf bin/

