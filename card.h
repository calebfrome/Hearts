#ifndef CARD_H
#define CARD_H

#include <QObject>

class Card {
public:
    //explicit Card(QObject *parent = 0);

    enum Suit {S_CLUBS,S_DIAMONDS,S_SPADES,S_HEARTS};

    Card();
    Card(int rank, Suit suit);
    Card(int rank, int suit);
    bool operator==(Card card);
    Suit getSuit();
    int getSuitInt();
    QString getSuitString();
    int getRank();
    QString getRankString();
    void setSuit(Suit suit);
    void setRank(int rank);
    bool isCard(int rank, int suit);
    bool isCard(Card card);
    QString toString();

private:
    Suit _suit;
    int _rank;

//signals:

//public slots:

};

#endif // CARD_H
