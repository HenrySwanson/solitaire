#ifndef SOLITAIRE_TABLE_H
#define SOLITAIRE_TABLE_H

#include <deque>
#include "card.h"

/** Represents a table on which solitaire is played */
class Table {
  public:

    /** Default constructor, deals out the card piles */
    Table();

    /** Default destructor */
    ~Table();

    /** Takes up to three cards from deck and puts them in the tray */
    bool drawNext3();

    /** Puts the tray back into the deck */
    bool resetTray();

    /** Moves a card from a pile to an ace pile */
    bool movePileToAce(int src, int dest);

    /** Moves cards from one pile to another */
    bool movePileToPile(int src, int dest, int num);

    /** Moves the top card of the tray to an ace pile */
    bool moveTrayToAce(int dest);

    /** Moves top card of tray to a pile */
    bool moveTrayToPile(int dest);

    /** Returns number of cards in deck */
    unsigned char getDeckLength();

    /** Returns the top three (or fewer) cards in the tray */
    std::deque<Card> getTrayTop();

    /** Returns the top card on the ith ace pile */
    const Card* getTopOfAcePile(int i);

    /** Returns the face-up cards on the ith pile */
    std::deque<Card> getFlippedPile(int i);

    /** Returns how many unflipped cards in the ith pile */
    unsigned char getUnflippedCards(int i);

    /** Returns the card hidden by the tray */
    Card* getUnderCard();

    /** Checks if the game is won */
    bool gameIsWon();

  private:
  
    /** The deck of cards */
    std::deque<Card> deck_;

    /** The just-drawn from the deck cards */
    std::deque<Card> tray_;

    /** An array of 4 piles for ace piles */
    std::deque<Card> aces_ [4];

    /** An array of 7 piles for the normal piles */
    std::deque<Card> piles_ [7];

    /** How many cards are flipped over in the normal piles */
    unsigned char unflippedCards_ [7];

    unsigned int numShownInTray_;

    /** Creates a shuffled deck */
    Card* makeDeck();

    /** Deals out the cards onto the stacks */
    void setup();

    /** Checks if suits and ranks work for moving to an ace pile */
    bool canMoveToAce(std::deque<Card> srcPile, std::deque<Card> destPile);

    /** Checks if suits and ranks work for moving from pile to pile */
    bool canMoveToPile(std::deque<Card> srcPile, std::deque<Card> destPile, int howMany);

  public: // DEBUGGING
    void printTable();
    void printDeque(const char* const name, std::deque<Card> cardPile, int index = -1);

};

#endif
