#include "table.h"

#include <iostream>
#include <ctime>
#include <cstdlib>

Table::Table() {
    numShownInTray_ = 0;
    setup();
}

Table::~Table() {

}

bool Table::drawNext3() {
    if(deck_.size() == 0) return false;
    numShownInTray_ = 0;
    for(int i = 0; i < 3 && deck_.size() != 0; i++) {
        numShownInTray_++;
        tray_.push_front(deck_.front());
        deck_.pop_front();
    }
    return true;
}

bool Table::resetTray() {
    if(deck_.size() != 0) return false; //can only do if deck is empty
    int size = tray_.size();
    for(int i = 0; i < size; i++) { //puts tray into deck
        deck_.push_front(tray_.front());
        tray_.pop_front(); //TODO can i just call reverse and copy it over?
    }
    numShownInTray_ = 0;
    return true;
}

bool Table::movePileToAce(int src, int dest) {

    if(src < 0 || src >= 7 || dest < 0 || dest >= 4) return false;
    std::deque<Card>& srcPile = piles_[src];
    std::deque<Card>& destPile = aces_[dest];

    if(srcPile.size() == 0) return false;
    if(!canMoveToAce(srcPile, destPile)) return false;

    destPile.push_front(srcPile.front());
    srcPile.pop_front();
    if(unflippedCards_[src] == srcPile.size() && unflippedCards_[src] != 0) {
            unflippedCards_[src]--; //if it was the last card flipped and not the last card in the stack
    }
    return true;
}

bool Table::movePileToPile(int src, int dest, int howMany) {

    if(src < 0 || src >= 7 || dest < 0 || dest >= 7 || src == dest) return false;

    std::deque<Card>& srcPile = piles_[src];
    std::deque<Card>& destPile = piles_[dest];
    std::deque<Card> temp;

    if(unflippedCards_[src] > srcPile.size() - howMany) return false; //If trying to flip more cards than are faceup
    if(!canMoveToPile(srcPile, destPile, howMany)) return false;

    for(int i = 0; i < howMany; i++) { //loads cards into temp stack
        temp.push_front(srcPile.front());
        srcPile.pop_front();
    }
    for(int i = 0; i < howMany; i++) { //puts from temp into dest
        destPile.push_front(temp.front());
        temp.pop_front();
    }

    if(unflippedCards_[src] == srcPile.size() && unflippedCards_[src] != 0)
        unflippedCards_[src]--; //if it was the last faceup card on the pile

    return true;
}

bool Table::moveTrayToAce(int dest) {

    if(dest < 0 || dest > 3) return false;
    if(tray_.size() == 0) return false;

    std::deque<Card>& destPile = aces_[dest];
    if(!canMoveToAce(tray_, destPile)) return false;

    destPile.push_front(tray_.front());
    tray_.pop_front();
    if(numShownInTray_ != 1) numShownInTray_--;
    if(tray_.empty()) numShownInTray_ = 0;
    return true;
}

bool Table::moveTrayToPile(int dest) {

    if(dest < 0 || dest > 6) return false;
    if(tray_.size() == 0) return false;

    std::deque<Card>& destPile = piles_[dest];
    if(!canMoveToPile(tray_, destPile, 1)) return false;

    destPile.push_front(tray_.front());
    tray_.pop_front();
    if(numShownInTray_ != 1) numShownInTray_--;
    if(tray_.empty()) numShownInTray_ = 0;

    return true;
}

unsigned char Table::getDeckLength() {
    return deck_.size();
}

std::deque<Card> Table::getTrayTop() {
    std::deque<Card> trayCopy = tray_;

    while(trayCopy.size() != numShownInTray_) trayCopy.pop_back();

    return trayCopy;
}

const Card* Table::getTopOfAcePile(int i) {
    if(aces_[i].empty()) return NULL;
    return &aces_[i].front();
}

std::deque<Card> Table::getFlippedPile(int i) {
    std::deque<Card> pileCopy = piles_[i];

    int unflipped = unflippedCards_[i];
    for(int j = 0; j < unflipped; j++) pileCopy.pop_back(); //removes the unflipped cards

    return pileCopy;
}

unsigned char Table::getUnflippedCards(int i) {
    return unflippedCards_[i];
}

Card* Table::getUnderCard() {
    if(tray_.size() < 2) return NULL;
    Card topCard = tray_.front();
    tray_.pop_front();
    Card* pSecondCard = &tray_.front();
    tray_.push_front(topCard);
    return pSecondCard;
}

bool Table::gameIsWon() {
    for(int i = 0; i < 4; i++) {
        if(aces_[i].empty() || aces_[i].front().getRankNum() != 13) return false; //if the top is not a king
    }
    return true;
}

Card* Table::makeDeck() {

    Card* cards = new Card[52];   //Makes deck
    int index = 0;
    for(int suit = 0; suit < 4; suit++) {
        for(int rank = 1; rank <= 13; rank++) {
            cards[index++] = Card(rank, suit);
        }
    }

    static int x = 0; //prevents identical srand calls when called within a second
    srand((int) time(0) + x++);         //shuffles
    for(int i = 0; i < 52; i++) {
        Card temp = cards[i];
        int swapWith = i + rand() % (52 - i); //current index or later
        cards[i] = cards[swapWith];
        cards[swapWith] = temp;
    }
    return cards;
}

void Table::setup() {

    Card* cards = makeDeck();
    int index = 0;

    for(int pile = 0; pile < 7; pile++) {
        std::deque<Card>& dc = piles_[pile];
        for(int i = 0; i <= pile; i++) {
            dc.push_front(cards[index++]); //pushes all the Cards
            unflippedCards_[i] = i;  //sets number of facedown cards
        }
    }

    for(int i = 28; i < 52; i++) deck_.push_front(cards[index++]);

    delete[] cards;
}

bool Table::canMoveToAce(std::deque<Card> srcPile, std::deque<Card> destPile) {

    Card targetCard = srcPile.front();
    int topCardRank = 0;

    if(destPile.size() != 0) { //If dest is empty, all that matters is that it's an ace
        Card topCard = destPile.front();
        if(topCard.getSuitNum() != targetCard.getSuitNum()) return false; //suit only matters if non-empty
        topCardRank = topCard.getRankNum();
    }
    return (topCardRank + 1 == targetCard.getRankNum()); //checks if ranks match (suit is checked above)
}

bool Table::canMoveToPile(std::deque<Card> srcPile, std::deque<Card> destPile, int howMany) {

    for(int i = 0; i < howMany - 1; i++) srcPile.pop_front();

    Card secondCard = srcPile.front();
    if(destPile.size() == 0) return (secondCard.getRankNum() == 13); //if the destination is empty, only a king can go.

    Card topCard = destPile.front();

    if(topCard.getRankNum() - secondCard.getRankNum() != 1) return false; //if rank doesn't match
    if(topCard.getSuitNum() == secondCard.getSuitNum()) return false;     //if suits are the same
    if(topCard.getSuitNum() + secondCard.getSuitNum() == 3) return false; //if suits are the same color
    return true;
}

#ifdef DEBUG

void Table::printTable() {
    std::cout << "Table:" << std::endl;
    printDeque("Deck", deck_);
    printDeque("Tray", tray_);
    for(int i = 0; i < 4; i++) printDeque("Ace", aces_[i], i);
    for(int i = 0; i < 7; i++) printDeque("Pile", piles_[i], i);
    std::cout << "Num in tray: " << numShownInTray_;
}

void Table::printDeque(const char* const name, std::deque<Card> temp, int index) {
    std::cout << name;
    if(index != -1) std::cout << " #" << index;
    std::cout << ": ";
    int i = temp.size();
    while(i != 0) {
        i--;
        const Card topCard = temp.front();
        std::cout << topCard.getRank() << topCard.getSuit() << " ";
        temp.pop_front();
        if(index != -1 && i == unflippedCards_[index]) std::cout << "| ";
    }
    std::cout << std::endl;
}

#else

inline void Table::printTable() {}
inline void Table::printDeque(const char* const name, std::deque<Card> temp, int index) {}

#endif
