#ifndef SOLITAIRE_CARD_H
#define SOLITAIRE_CARD_H

/** Represents a card from a standard deck */
class Card {
  public:

    /** Default constructor. It makes garbage! */
    Card();

    /** Constructs a Card with rank and suit. */
    Card(int rank, int suit);

    /** Copy constructor. */
    Card(const Card& card);

    /** Default destructor, nothing special. */
    ~Card();

    /** Returns the character rank of the card. T is 10. */
    char getRank() const;

    /** Returns the numerical rank of the card. */
    int getRankNum() const;

    /** Returns the suit of the card. */
    char getSuit() const;

    /** Returns the numerical suit of the card. */
    int getSuitNum() const;

  private:

    /** A bitfield containing rank and suit. */
    unsigned char ID_;
};

#endif
