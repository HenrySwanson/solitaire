#ifndef SOLITAIRE_WINDOW_H
#define SOLITAIRE_WINDOW_H

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"

#include "card.h"
#include "table.h"

class Window {
  public:

    /** Creates a window to be displayed on screen */
    Window(SDL_Surface*& screen);

    /** Default destructor */
    ~Window();

    /** Initializes the window, including loading images */
    bool init();

    /** Handles a mousedown event */
    void mouseDown(int x, int y);

    /** Handles a mousedown event */
    void mouseUp(int x, int y);

    /** Blits the contents of the window to screen */
    void display();

    static const int WINDOW_WIDTH = 577;
    static const int WINDOW_HEIGHT = 520;

  private:
    static const int CARD_WIDTH = 71;
    static const int CARD_HEIGHT = 95;

    static const int DECK_X = 10;
    static const int DECK_Y = 10;

    static const int TRAY_X = 120;
    static const int TRAY_Y = 10;
    static const int TRAY_SEP = 20;

    static const int PILE_X = 10;
    static const int PILE_Y = 115;
    static const int PILE_SEP_X = 81;
    static const int PILE_SEP_Y_UNFLIPPED = 10;
    static const int PILE_SEP_Y_FLIPPED = 20;

    static const int ACES_X = 253;
    static const int ACES_Y = 10;
    static const int ACES_SEP = 81;

    static const int BUTTON_X = 10;
    static const int BUTTON_Y = 470;
    static const int BUTTON_WIDTH = 125;
    static const int BUTTON_HEIGHT = 40;

    /** The table being displayed */
    Table table_;

    /** The location of the last mouse down event */
    int lastMouseDown_;

    /** If a pile was clicked, how many cards are selected. Otherwise, 0. */
    int numSelectedInPile_;

    /** Whether a card is being dragged */
    bool dragged_;

    /** If a card is being dragged, where it's upper left corner is */
    SDL_Rect draggedOffset_;

    /** The screen to be blitted to */
    SDL_Surface* screen_;

    /** The image for the back of a card */
    SDL_Surface* cardBack_;

    /** The images for the faces of the cards */
    SDL_Surface* cardFaces_ [52];

    /** The images for an empty space */
    SDL_Surface* emptySpace_;

    /** The images for the buttons */
    SDL_Surface* newGame_;

    /** Loads an individual image, converts from 24 to 32 bit, and adds alpha */
    SDL_Surface* load32BitImage(const char* const name);

    /** Checks if x and y are between the given bounds */
    bool isInBounds(int x, int y, int minX, int maxX, int minY, int maxY);

    /** Returns the image for the card */
    SDL_Surface* getCardImage(Card c);

    /** Converts a click location into an int representing a card location */
    int computeClickLocation(int x, int y, bool mouseIsDown);

    /** Determines if the click was on the deck */
    bool clickedOnDeck(int x, int y);

    /** Determines if the click was on the tray, and sets dragging variables */
    bool clickedOnTray(int x, int y, bool mouseIsDown);

    /** Determines if the click was on the ace piles */
    int clickedOnAces(int x, int y);

    /** Determines if the click was on the piles, and sets dragging variables */
    int clickedOnPile(int x, int y, bool mouseIsDown);

    /** Determines if the click was on the buttons */
    bool clickedOnButton(int x, int y);

    /** Determines where on the stack the mouse was clicked */
    int setLastPileIndex(int x, int y);

    /** Blits background */
    void drawBackground();

    /** Blits deck */
    void drawDeck();

    /** Blits tray */
    void drawTray();

    /** Blits piles */
    void drawPiles();

    /** Blits ace piles */
    void drawAces();

    /** Blits buttons */
    void drawButtons();

    /** Blits the card currently being dragged */
    void drawDraggedCard();

  public: // DEBUGGING
    void logAction();
};

#endif
