#include "window.h"

Window::Window(SDL_Surface*& screen) {
    screen_ = screen;
    lastMouseDown_ = 0;
    numSelectedInPile_ = 0;
    dragged_ = false;
}

Window::~Window() {
    SDL_FreeSurface(cardBack_);
    SDL_FreeSurface(emptySpace_);
    for(int i = 0; i < 52; i++) SDL_FreeSurface(cardFaces_[i]);
}

bool Window::init() {
    cardBack_ = load32BitImage("images/back.png");
    emptySpace_  = load32BitImage("images/empty.png");
    newGame_ = load32BitImage("images/newgame.png");
    int index = 0;
    for(int suit = 0; suit < 4; suit++) {
        for(int rank = 1; rank <= 13; rank++) {
            Card c (rank, suit);
            char pc [] = {'i', 'm', 'a', 'g', 'e', 's', '/', c.getRank(), c.getSuit(), '.', 'p', 'n', 'g', '\0'}; //easier than stringstreams
            SDL_Surface* cardImage = load32BitImage(pc);
            if(cardImage == NULL) return false;
            cardFaces_[index++] = cardImage;
        }
    }
    return true;
}

void Window::mouseDown(int x, int y) {
    lastMouseDown_ = computeClickLocation(x, y, true);
}

void Window::mouseUp(int x, int y) {
    logAction();
    int lastMouseUp = computeClickLocation(x, y, false);
    int acePileNum = 0; //to avoid crossing initialization in cases
    int pileNum = 0;
    int srcPileNum = 0;
    dragged_ = false;
    switch(lastMouseUp) {
        case 1: //Deck
            if(lastMouseDown_ == 1) {
                if(!table_.drawNext3()) table_.resetTray();
            }
            break;
        case 2: //Dragging to the tray does nothing
            break;
        case 3: //Ace piles
        case 4:
        case 5:
        case 6:
            acePileNum = lastMouseUp - 3;
            if(lastMouseDown_ == 2) table_.moveTrayToAce(acePileNum);
            if(7 <= lastMouseDown_ && lastMouseDown_ <= 13) { //if last click was on pile
                pileNum = lastMouseDown_ - 7;
                if(numSelectedInPile_ == 1) table_.movePileToAce(pileNum, acePileNum);
            }
            break;
        case 7: //Piles
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
            if(lastMouseDown_ == lastMouseUp) break;
            pileNum = lastMouseUp - 7;
            if(lastMouseDown_ == 2) table_.moveTrayToPile(pileNum);
            if(7 <= lastMouseDown_ && lastMouseDown_ <= 13) {
                srcPileNum = lastMouseDown_ - 7;
                if(numSelectedInPile_ > 0)
                	table_.movePileToPile(srcPileNum, pileNum, numSelectedInPile_);
            }
            break;
        case 14:
            if(lastMouseDown_ == lastMouseUp)
            	table_ = Table();
            break;
        default:
            break;
    }
    lastMouseDown_ = 0;
}

void Window::display() {
    drawBackground();
    drawDeck();
    drawTray();
    drawPiles();
    drawAces();
    drawButtons();
    drawDraggedCard();
}

SDL_Surface* Window::load32BitImage(const char* const name) {
    SDL_Surface* img24bit = IMG_Load(name);
    if(img24bit == NULL) return NULL;
    SDL_Surface* img32bit = SDL_DisplayFormat(img24bit);
    SDL_FreeSurface(img24bit);
    SDL_SetColorKey(img32bit, SDL_SRCCOLORKEY,  SDL_MapRGB(screen_->format, 0, 0xFF, 0xFF));
    return img32bit;
}

inline bool Window::isInBounds(int x, int y, int minX, int maxX, int minY, int maxY) {
    return (minX <= x && x < maxX && minY <= y && y < maxY);
}

SDL_Surface* Window::getCardImage(Card c) {
    return cardFaces_[c.getSuitNum() * 13 + c.getRankNum() - 1]; //the -1 is because there is no card rank 0
}

int Window::computeClickLocation(int x, int y, bool mouseIsDown) {

    if(clickedOnDeck(x, y)) return 1;

    if(clickedOnTray(x, y, mouseIsDown)) return 2;

    int result = clickedOnAces(x, y);
    if(result != -1) return result + 3;

    result = clickedOnPile(x, y, mouseIsDown);
    if(result != -1) return result + 7;

    if(clickedOnButton(x, y)) return 14;

    return 0;
}

bool Window::clickedOnDeck(int x, int y) {
    int leftEdge = DECK_X + table_.getDeckLength();
    return isInBounds(x, y, leftEdge, leftEdge + CARD_WIDTH, DECK_Y, DECK_Y + CARD_HEIGHT);
}

bool Window::clickedOnTray(int x, int y, bool mouseIsDown) {
    int traySize = table_.getTrayTop().size();
    int leftEdge = TRAY_X + TRAY_SEP * (traySize - 1);
    if(isInBounds(x, y, leftEdge, leftEdge + CARD_WIDTH, TRAY_Y, TRAY_Y + CARD_HEIGHT)) {
        if(traySize != 0 && mouseIsDown) {
            dragged_ = true;
            draggedOffset_.x = TRAY_X + TRAY_SEP * (traySize - 1) - x;
            draggedOffset_.y = TRAY_Y - y;
        }
        return true;
    }
    return false;
}

int Window::clickedOnAces(int x, int y) {
    for(int i = 0; i < 4; i++) {
        int leftEdge = ACES_X + ACES_SEP * i;
        if(isInBounds(x, y, leftEdge, leftEdge + CARD_WIDTH, ACES_Y, ACES_Y + CARD_HEIGHT)) return i; //if on ace pile
    }
    return -1;
}

int Window::clickedOnPile(int x, int y, bool mouseIsDown) {
    for(int i = 0; i < 7; i++) {
        int leftEdge = PILE_X + PILE_SEP_X * i;
        int noFlipped = table_.getFlippedPile(i).size();
        int minY = PILE_Y + table_.getUnflippedCards(i) * PILE_SEP_Y_UNFLIPPED;
        int maxY = minY + CARD_HEIGHT;
        if(noFlipped != 0) maxY += (noFlipped - 1) * PILE_SEP_Y_FLIPPED;

        if(!isInBounds(x, y, leftEdge, leftEdge + CARD_WIDTH, minY, maxY)) continue;
        if(mouseIsDown) {
            dragged_ = true;
            numSelectedInPile_ = setLastPileIndex(x, y);
            draggedOffset_.x = leftEdge - x;
            draggedOffset_.y = minY + (noFlipped - numSelectedInPile_) * PILE_SEP_Y_FLIPPED - y;
        }
        return i;
    }
    return -1;
}

bool Window::clickedOnButton(int x, int y) {
    if(isInBounds(x, y, BUTTON_X, BUTTON_X + BUTTON_WIDTH, BUTTON_Y, BUTTON_Y + BUTTON_HEIGHT)) return true;
    return false;
}

int Window::setLastPileIndex(int x, int y) {
    for(int i = 0; i < 7; i++) {
        int leftEdge = PILE_X + PILE_SEP_X * i;
        int noFlipped = table_.getFlippedPile(i).size();
        int minY = PILE_Y + table_.getUnflippedCards(i) * PILE_SEP_Y_UNFLIPPED;
        int maxY = minY + (noFlipped - 1) * PILE_SEP_Y_FLIPPED + CARD_HEIGHT;

        if(!isInBounds(x, y, leftEdge, leftEdge + CARD_WIDTH, minY, maxY)) continue;
        numSelectedInPile_ = noFlipped - ( (y - minY) / PILE_SEP_Y_FLIPPED);
        return (numSelectedInPile_ <= 0) ? 1 : numSelectedInPile_; //if the card that is fully exposed is clicked
    }
    return 0;
}

void Window::drawBackground() {
    SDL_FillRect(screen_, &screen_->clip_rect, SDL_MapRGB(screen_->format, 0x7F, 0x7F, 0x7F));

    SDL_Rect offset;
    offset.x = DECK_X;
    offset.y = DECK_Y;
    SDL_BlitSurface(emptySpace_, NULL, screen_, &offset);

    for(int i = 0; i < 4; i++) {
        offset.x = ACES_X + ACES_SEP * i;
        offset.y = ACES_Y;
        SDL_BlitSurface(emptySpace_, NULL, screen_, &offset);
    }

    for(int i = 0; i < 7; i++) {
        offset.x = PILE_X + PILE_SEP_X * i;
        offset.y = PILE_Y;
        SDL_BlitSurface(emptySpace_, NULL, screen_, &offset);
    }
}

void Window::drawDeck() {
    int deckSize = table_.getDeckLength();
    SDL_Rect offset;
    for(int i = 0; i < deckSize; i++) {
        offset.x = DECK_X + i / 2 * 2;
        offset.y = DECK_Y;
        SDL_BlitSurface(cardBack_, NULL, screen_, &offset);
    }
}

void Window::drawTray() {
    std::deque<Card> trayCopy = table_.getTrayTop();

    if(dragged_ && lastMouseDown_ == 2) {
        trayCopy.pop_front(); //so the top is not drawn
        if(trayCopy.empty()) { //shows the card under the currently dragged card, if it's the last on in the visible tray
            SDL_Rect offset;
            offset.x = TRAY_X;
            offset.y = TRAY_Y;
            Card* underCard = table_.getUnderCard();
            if(underCard == NULL) return; //if the whole tray is empty
            SDL_Surface* underCardSurface = getCardImage(*underCard);
            SDL_BlitSurface(underCardSurface, NULL, screen_, &offset);
            return;
        }
    }

    SDL_Rect offset;
    for(int i = 0; !trayCopy.empty(); i++) {
        Card c = trayCopy.back();
        trayCopy.pop_back();
        offset.x = TRAY_X + TRAY_SEP * i;
        offset.y = TRAY_Y;
        SDL_BlitSurface(getCardImage (c), NULL, screen_, &offset);
    }
}

void Window::drawPiles() {
    SDL_Rect offset;
    offset.x = PILE_X;

    for(int pileNum = 0; pileNum < 7; pileNum++) {
        offset.y = PILE_Y;

        int unflipped = table_.getUnflippedCards(pileNum);   //draws unflipped
        for(int i = 0; i < unflipped; i++) {
            SDL_BlitSurface(cardBack_, NULL, screen_, &offset);
            offset.y += PILE_SEP_Y_UNFLIPPED;
        }

        std::deque<Card> pileCopy = table_.getFlippedPile(pileNum); //draws flipped

        if(dragged_ && lastMouseDown_ == pileNum + 7) {
            for(int i = 0; i < numSelectedInPile_; i++) pileCopy.pop_front();
        }

        while(!pileCopy.empty()) {
            Card c = pileCopy.back();
            pileCopy.pop_back();
            SDL_BlitSurface(getCardImage(c), NULL, screen_, &offset);
            offset.y += PILE_SEP_Y_FLIPPED;
        }

        offset.x += PILE_SEP_X;
    }
}

void Window::drawAces() {
    SDL_Rect offset;
    offset.x = ACES_X;
    offset.y = ACES_Y;

    for(int acePileNum = 0; acePileNum < 4; acePileNum++) {
        const Card* c = table_.getTopOfAcePile (acePileNum);
        if(c != NULL) SDL_BlitSurface(getCardImage(*c), NULL, screen_, &offset);
        offset.x += ACES_SEP;
    }
}

void Window::drawButtons() {
    SDL_Rect offset;
    offset.x = BUTTON_X;
    offset.y = BUTTON_Y;
    SDL_BlitSurface(newGame_, NULL, screen_, &offset);
}

void Window::drawDraggedCard() {
    if(dragged_) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        SDL_Rect offset;
        SDL_Rect modifiableCopy;
        offset.x = x + draggedOffset_.x;
        offset.y = y + draggedOffset_.y;

        if(lastMouseDown_ == 2) {
            SDL_Surface* draggedCard = getCardImage(table_.getTrayTop().front() );
            SDL_BlitSurface(draggedCard, NULL, screen_, &offset);

        } else { //if on pile
            std::deque<Card> pileCopy = table_.getFlippedPile(lastMouseDown_ - 7);

            while((signed int) pileCopy.size() != numSelectedInPile_) pileCopy.pop_back();

            while(!pileCopy.empty()) {
                SDL_Surface* draggedCard = getCardImage(pileCopy.back() );
                pileCopy.pop_back();
                modifiableCopy = SDL_Rect(offset);
                SDL_BlitSurface(draggedCard, NULL, screen_, &modifiableCopy); //changes modifiableCopy.x and .y to >= 0
                offset.y += PILE_SEP_Y_FLIPPED;
            }
        }
    }
}

#ifdef _DEBUG

void Window::logAction() {
    int x, y;
    SDL_GetMouseState(&x, &y);

    std::cout << "======================" << std::endl;
    std::cout << "Mouse up: " << x << ", " << y << std::endl;
    std::cout << "Last mouse down: " << lastMouseDown_ << std::endl;
    std::cout << "numSelectedInPile: " << numSelectedInPile_ << std::endl;
    std::cout << "Dragged: " << dragged_ << std::endl;
    std::cout << "Dragged Offset: " << draggedOffset_.x << ", " << draggedOffset_.y << std::endl;
    table_.printTable();
    std::cout << std::endl;
}

#else

inline void Window::logAction() {}

#endif
