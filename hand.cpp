#include "hand.h"

#include <QDebug>

//default constructor
Hand::Hand() {
    //_numRowsAlloc = 13;
    //_numRowsUsed = 0;
    //_hand = new Card[_numRowsAlloc];
}

//overloaded constructor
Hand::Hand(int numRows) {
    //_numRowsAlloc = numRows;
    //_numRowsUsed = 0;
    //_hand = new Card[_numRowsAlloc];
}

/*copy constructor
Hand::Hand(const Hand& newHand) {
    //_hand = newHand._hand;
    //_numRowsAlloc = newHand._numRowsAlloc;
    //_numRowsUsed = newHand._numRowsUsed;
}*/

//destuctor
Hand::~Hand() {
}

/*overloaded assignment operator
Hand Hand::operator=(Hand newHand) {
    _hand = newHand._hand;
    _numRowsAlloc = newHand._numRowsAlloc;
    _numRowsUsed = newHand._numRowsUsed;
    return *this;
}*/

//overloaded [] operator - indexing operator
Card Hand::operator[](int index) {
    if(index > _hand.size()-1) {
        qDebug() << "index out of range, too high: " << index;
        return _hand[_hand.size()-1];
    }
    if(index < 0) {
        qDebug() << "index out of range, too low: " << index;
        return _hand[0];
    }
    return _hand[index];
}

//add a card to the end of the hand
Card Hand::addCard(Card card) {

    //set the next (first unused) element of _hand to card
    //*(_hand + _numRowsUsed) = card;
    //_hand[_numRowsUsed] = card; //debug
    //_hand[0] = card; //debug

    _hand.append(card);

    //_numRowsUsed++;
    return card;
}

//set the specified index to the given card
void Hand::set(int index, Card card) {
    //*(_hand + index) = card;
    _hand[index] = card;
}

//remove the specified card from the hand
Card Hand::removeCardAt(int index) {
    return _hand.takeAt(index);
    /*
    Card removedCard = at(index);
    //elements which were to the right of the removed card are moved one spot to the left
    for (int i = index;i < _numRowsUsed;i++) {
        _hand[i] = _hand[i + 1];
    }
    //set last element to an empty card
    _hand[_numRowsUsed - 1] = Card();
    //number of cards stored is one fewer
    _numRowsUsed--;
    return removedCard;*/
}

//remove the specified card from the hand
Card Hand::removeCard(Card card) {
    return _hand.takeAt(_hand.indexOf(card));
    /*
    Card removedCard = card;
    Card testCard; //debug
    int index = indexOfCard(card);
    //elements which were to the right of the removed card are moved one spot to the left
    for (int i = index;i < _numRowsUsed-1;i++) {
        testCard = _hand[i];
        _hand[i] = _hand[i + 1];
        testCard = _hand[i];
    }
    //set last element to an empty card
    _hand[_numRowsUsed - 1] = Card();
    //number of cards stored is one fewer
    _numRowsUsed--;
    return removedCard;*/
}

//remove three specified cards from the hand
void Hand::removeThreeCards(Card card1, Card card2, Card card3) {
    removeCard(card1);
    removeCard(card2);
    removeCard(card3);
}

//clear all items from the database
void Hand::clear() {
    _hand.clear();
    /*
    for (int i = _numRowsUsed-1; i >= 0; i--) {
        removeCardAt(i);
    }*/
}

//return the card at a given index
Card Hand::at(int index) {
    return _hand.at(index);
    //return *(_hand + index);
}

//return the number of cards currently stored in the hand
int Hand::numCards() {
    return _hand.size();
    //return _numRowsUsed;
}

//return whether this hand contains the given card
bool Hand::contains(Card card) {

    return _hand.contains(card);
    /*
    for(int i=0;i<numCards();i++) {
        if(this->at(i) == card) {
            return true;
        }
    }
    return false;*/
}

//return whether this hand contains the card of the given rank and suit
bool Hand::contains(int rank, int suit) {

    return _hand.contains(Card(rank, suit));
    /*
    for(int i=0;i<numCards();i++) {
        if(this->at(i).getRank() == rank && this->at(i).getSuitInt() == suit) {
            return true;
        }
    }
    return false;*/
}


//return the index of the given card
int Hand::indexOfCard(int rank, int suit) {

    return _hand.indexOf(Card(rank, suit));
    /*
    for(int pos=0;pos<_numRowsUsed;pos++) {
        if(at(pos).getSuitInt()==suit && at(pos).getRank()==rank) {
            return pos;
        }
    }
    return -1;*/
}

//return the index of the given card
int Hand::indexOfCard(int rank, Card::Suit suit) {
    return indexOfCard(rank,int(suit));
}

//return the index of the given card
int Hand::indexOfCard(Card card) {
    return _hand.indexOf(card);
    //return indexOfCard(card.getRank(),card.getSuitInt());
}

//return the number of cards in the given suit
int Hand::numInSuit(int suit) {
    int num=0;
    for(int pos=0; pos<_hand.size(); pos++) {
        if(at(pos).getSuitInt()==suit) {
            num++;
        }
    }
    return num;
}

//return the rank of the highest card in the given suit
int Hand::highestInSuitRank(int suit) {
    for(int pos=_hand.size()-1; pos>=0; pos--) {
        if(at(pos).getSuitInt()==suit) {
            return at(pos).getRank();
        }
    }
    return -1;
}

//return the position of the highest card in the given suit
int Hand::highestInSuitPos(int suit) {
    for(int pos=_hand.size()-1;pos>=0;pos--) {
        if(at(pos).getSuitInt()==suit) {
            return pos;
        }
    }
    return -1;
}

//return the position of the highest card in the given suit whose rank is <= max
int Hand::highestInSuitPosMax(int suit, int max) {
    for(int pos=_hand.size()-1; pos>=0; pos--) {
        if(at(pos).getSuitInt()==suit && at(pos).getRank()<=max) {
            return pos;
        }
    }
    return -1;
}

//return the rank of the lowest card in the given suit
int Hand::lowestInSuitRank(int suit) {
    for(int pos=0; pos<_hand.size(); pos++) {
        if(at(pos).getSuitInt()==suit) {
            return at(pos).getRank();
        }
    }
    return -1;
}

//return the position of the lowest card in the given suit
int Hand::lowestInSuitPos(int suit) {
    for(int pos=0; pos<_hand.size(); pos++) {
        if(at(pos).getSuitInt()==suit) {
            return pos;
        }
    }
    return -1;
}

//INWORK determines which three cards the computer will pass (cards are passed by reference)
//and removes those cards from the hand
void Hand::getAIPass(Card &card1, Card &card2, Card &card3) {
    //trying to shoot or nah?

    //if shooting, identify weak spots (e.g. low hearts, big suits w/o highest cards) and places to capitalize

    //if not shooting, identify most dangerous cards, in this order:
    //  QKA-S if not deep in spades
    //  high hearts if not deep in hearts
    //  is clubs a bad suit?
    //  is diamonds a bad suit?


    //dummy cards for now, until I have time to write this method
    Hand cardsToPass = Hand(3);
    int slotsFilled = 0;
    //pass high spades first
    while(numInSuit(Card::S_SPADES)>0 && highestInSuitRank(Card::S_SPADES) > 11) {
        removeCard(cardsToPass.addCard(at(highestInSuitPos(Card::S_SPADES))));
        slotsFilled++;
    }
    //otherwise pass highest cards (probably high hearts)
    while(slotsFilled<3) {
        removeCard(cardsToPass.addCard(at(12-slotsFilled)));
        slotsFilled++;
    }

    card1 = cardsToPass[0];
    card2 = cardsToPass[1];
    card3 = cardsToPass[2];
}

//convert card to string and send to output
void Hand::printCard(int index) {
    qDebug() << at(index).toString().toStdString().c_str();
}
