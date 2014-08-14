#include <iostream>
#include <string>

#include "SDL/SDL.h"

#include "window.h"

/** Main... duh */
int main(int argc, char** argv);

/** Initializes SDL and the Window object */
bool init(Window*& w, SDL_Surface*& s);

/** Gets events and dispatches them to w */
int handleEvents(Window* w);

/** Tells w to do any internal computation, prior to display */
int handleLogic(Window* w);

/** Calls window w's display commands */
int handleDisplay(Window* w, SDL_Surface* screen);

/** Frees up memory, surfaces... */
void cleanup(Window* w);
