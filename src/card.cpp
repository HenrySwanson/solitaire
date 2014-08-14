#include "card.h"

Card::Card() {
    ID_ = 0;
}

Card::Card(const Card& card) {
    ID_ = card.ID_;
}

Card::Card(int rank, int suit) {
    ID_ = ((rank & 0xF) << 2) | (suit & 0x3);
}

Card::~Card() {}

char Card::getRank() const {
    int rank = getRankNum();
    if(2 <= rank && rank <= 9) return '0' + rank;
    if(1 == rank) return 'A';
    if(10 == rank) return 'T';
    if(11 == rank) return 'J';
    if(12 == rank) return 'Q';
    if(13 == rank) return 'K';
    return 'X';
}

int Card::getRankNum() const {
    return (ID_ >> 2) & 0xF;
}

char Card::getSuit() const {
    switch(getSuitNum()) {
        case 0:
            return 'C';
        case 1:
            return 'D';
        case 2:
            return 'H';
        case 3:
            return 'S';
        default:
            return '0';
    }
}

int Card::getSuitNum() const {
    return (ID_ & 0x3);
}
