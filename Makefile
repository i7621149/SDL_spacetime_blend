OBJECTS = main.o Scene.o
HEADERS = Scene.hpp
COMPILER = clang++
PROGNAME = SDL_test
OUTPUT = -o
OPTIONS = -Wall -c -std=c++11

$(PROGNAME): $(OBJECTS)
	$(COMPILER) $(OBJECTS) -Wall -l SDL2 -l SDL2_image -l SDL2_ttf $(OUTPUT) $(PROGNAME)
	echo DONE!

main.o: main.cpp $(HEADERS)
	$(COMPILER) $(OPTIONS) main.cpp

Scene.o: Scene.cpp $(HEADERS)
	$(COMPILER) $(OPTIONS) Scene.cpp
