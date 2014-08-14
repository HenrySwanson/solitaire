#include "solitaire.h"

#include <iostream>

#include "SDL/SDL_image.h"

int main(int argc, char** argv) {
	std::cout << "Start" << std::endl;
    Window* gameWindow = NULL;
    SDL_Surface* screen = NULL;
    if(!init(gameWindow, screen)) {
    	std::cout << "Fail" << std::endl;
    	return -1;
    }

    int runningCode = 0; //anything else signifies error or quit
    while(runningCode == 0) {
        runningCode = handleEvents(gameWindow);
        if(runningCode != 0) break;
        runningCode = handleLogic(gameWindow);
        if(runningCode != 0) break;
        runningCode = handleDisplay(gameWindow, screen);
    }

    cleanup(gameWindow);
    return 0;
}

bool init(Window*& w, SDL_Surface*& s) {
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1) return false;
    SDL_WM_SetCaption("Solitaire", NULL);
    s = SDL_SetVideoMode(Window::WINDOW_WIDTH, Window::WINDOW_HEIGHT, 32, SDL_SWSURFACE);  //x = pile x + 7*width + 6*pile sep + buffer
    w = new Window(s);                                  //y = pile y + 6*unflip sep + 12*flip sep + height + buffer
    if(!w->init()) return false;
    return true;
}

int handleEvents(Window* w) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) return 1;

        if(event.type == SDL_MOUSEBUTTONDOWN) {
            if(event.button.button == SDL_BUTTON_LEFT) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                w->mouseDown(x, y);
            }
        }

        if(event.type == SDL_MOUSEBUTTONUP) {
            if(event.button.button == SDL_BUTTON_LEFT) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                w->mouseUp(x, y);
            }
        }
    }
    return 0;
}

int handleLogic(Window* w) {
    return 0;
}

int handleDisplay(Window* w, SDL_Surface* screen) {
    w->display();
    SDL_Flip(screen);
    return 0;
}

void cleanup(Window* w) {
    delete w;
    SDL_Quit();
}
