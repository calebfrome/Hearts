#ifndef HAND_H
#define HAND_H

#include "card.h"
#include <string>
#include <iostream>

class Hand {

public:

    Hand();
    Hand(int numRows);
    //Hand(const Hand& newHand);
    virtual ~Hand();
    //Hand operator=(Hand newHand);
    Card operator[](int index);
    Card addCard(Card card);
    void set(int index, Card card);
    Card removeCardAt(int index);
    Card removeCard(Card card);
    void removeThreeCards(Card card1, Card card2, Card card3);
    void clear();
    Card at(int index);
    int numCards();
    bool contains(Card card);
    bool contains(int rank, int suit);
    int indexOfCard(int rank, int suit);
    int indexOfCard(int rank, Card::Suit suit);
    int indexOfCard(Card card);
    int numInSuit(int suit);
    int highestInSuitRank(int suit);
    int highestInSuitPos(int suit);
    int highestInSuitPosMax(int suit, int max);
    int lowestInSuitRank(int suit);
    int lowestInSuitPos(int suit);
    void getAIPass(Card &card1, Card &card2, Card &card3);
    void printCard(int index);

private:
    QList<Card> _hand;
    //int _numRowsAlloc; // number of slots allocated in memory
    //int _numRowsUsed;  // number of memory slots which contain country data

    // internal methods
    //void manageMemory();
};

#endif // Hand_H
