#include "card.h"
#include <QObject>


Card::Card() {
    _suit = (Suit)0;
    _rank = 0;
}

Card::Card(int rank, Suit suit) {
    _suit = suit;
    _rank = rank;
}

Card::Card(int rank, int suit) {
    _suit = (Suit)suit;
    _rank = rank;
}

bool Card::operator==(Card card) {
    return (getSuitInt() == card.getSuitInt() && getRank() == card.getRank());
}

Card::Suit Card::getSuit() {
    return _suit;
}

int Card::getSuitInt() {
    return (int)_suit;
}

QString Card::getSuitString() {
    switch(_suit) {
    case S_CLUBS:
        return "clubs";
    case S_DIAMONDS:
        return "diamonds";
    case S_SPADES:
        return "spades";
    case S_HEARTS:
        return "hearts";
    }
    return "";
}

int Card::getRank() {
    return _rank;
}

QString Card::getRankString() {
    if(_rank<=10) {
        return QString::number(_rank);
    }
    else if(_rank==11) {
        return "Jack";
    }
    else if(_rank==12) {
        return "Queen";
    }
    else if(_rank==13) {
        return "King";
    }
    else return "Ace";
}

void Card::setSuit(Card::Suit newSuit) {
    _suit=newSuit;
}

void Card::setRank(int newRank) {
    _rank=newRank;
}

bool Card::isCard(int rank, int suit) {
    return (_rank == rank && _suit == suit);
}

bool Card::isCard(Card card) {
    return isCard(card.getRank(),card.getSuitInt());
}

QString Card::toString() {
    QString result;
    QString rankStr;

    switch(_rank) {
    case 14:
        rankStr = "Ace";
        break;
    case 13:
        rankStr = "King";
        break;
    case 12:
        rankStr = "Queen";
        break;
    case 11:
        rankStr = "Jack";
        break;
    default:
        rankStr = QString::number(_rank);
    }

    switch(_suit) {
    case 0:
        result = rankStr + "-C";
        break;
    case 1:
        result = rankStr + "-D";
        break;
    case 2:
        result = rankStr + "-S";
        break;
    case 3:
        result = rankStr + "-H";
        break;
    }

    return result;
}
