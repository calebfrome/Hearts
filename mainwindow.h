#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QIcon>
#include "card.h"
#include "hand.h"
#include "array.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    enum PassDirection {PD_LEFT,PD_RIGHT,PD_ACROSS,PD_NONE};
    //Q_ENUM(PassDirection);
	
    //Information about the suit in one player's hand
    struct SuitStatus {
		int suit;			//the suit (0-3)
		int numCards;		//number of cards in the suit (0-13)
        int numVoid;		//number of other players known to be void in this suit
        bool S_LEAD;		//safe to lead
        bool D_ALLVOID;		//everyone else is void in this suit
        //bool ADVG_LEAD;	//<safe to lead, and> there is a distinct advantage from leading from it: priority lead
		bool D_HIGH;		//contains high cards with coverage that may not last
        bool D_ALLHIGH;		//contains high cards with insufficient coverage underneath
        bool D_QS;			//contains queen of spades with insufficient coverage underneath (spades only)
		bool D_AKS;			//contains ace and/or king of spades with insufficient coverage (spades only)
    };

    //Information that the computers know about each player based only on seeing what has been played
    struct PlayerInfo {
        int hasQueen;       //-1: does not have queen; 0: might have queen; 1: has queen
        bool shooting;      //attempting a moon shot
    };

signals:
    void quitLoop();

private slots:
    void playGame();
    void card_clicked();
    void on_passCardsButton_clicked();

private:
    Ui::MainWindow *ui;

    //player order: 0 = user; 1 = p1; 2 = p2; 3 = p3
    QString player0 = "Caleb";
    QString player1 = "Ezra";
    QString player2 = "Casey";
    QString player3 = "Bongstiane";

    //setup
    PassDirection passDirection;
    bool noPassRoundEnabled;
    int roundsPerPassCycle;
    QIcon iconArray[4];
    bool cardsPlayed[4][13];     //false if in someone's hand; true if played. suit/rank (rank is offset by 2)
    int unplayedSuitTotals[4];   //number of unplayed cards remaining in each suit
    bool knownVoid[4][4];        //true if known to be void based on gameplay (player, suit)
    PlayerInfo pInfo[4];         //playerInfo array
    SuitStatus suitStatus[4][4]; //SuitStatus 2D array (player, suit)

    //deckhands
    Hand deck;
    Hand hands[4];

    //gameplay
    int handSize=0;
    bool heartsBroken=false;
    int lead=0; //Determines who leads each trick (0-3)
    bool playSelection; //true if double-click signifies a play; false if a pass
    QList<QListWidgetItem*> cardsToPass; //stores the cards that the player queues to pass
    int myPass1=-1;
    int myPass2=-1;
    int myPass3=-1;
    int myCard=-1;
    Hand trickCards; //cards played in this trick, sorted by order of play
    int trickOrder[4]; //order of players in this trick
    int trickTaker; //player currently taking this trick

    //points taken by winning tricks each hand
    int handPoints[4]; //player
    int totalHandPoints=0;

    //total scores
    int scores[4]; //player

    //variables for stats
    int numRows = 1; //number of rows in the scores table (dynamic)
    int gamesPlayed=0;
    int wins=0;
    int seconds=0;
    int thirds=0;
    int lasts=0;
    double winningPercentage=0;
    int moonShots=0;
    int moonShotsAllowed=0;
    int queensTaken=0;

    //gameplay methods
    int randomInt(int low, int high);
    void newGame();
    void setNoPassRound();
    void createDeck();
    void dealCards();
    void sortAllHands();
    void showAllHands(); //debug
    void showHand(int player); //debug
    void passCards();
    void sort3Ints(int &int1, int &int2, int &int3);
    void playTrick();
    QListWidgetItem* myTurn(Card &card);
    Card playCard(int player);
    void updatePlayerInfo(int player);
    SuitStatus getSuitStatus(Hand cHand, int suit);
    int dumpCard(int player, int leadSuit);
    int getCardLocation(int rank, int suit);
    int getCardLocation(int rank, Card::Suit suit);
    int getCardLocation(Card card);
    int playerShooting();
    int playerShooting(int threshold);
    void updateScores();
    void setVerticalScoreHeaders();
};

#endif // MAINWINDOW_H
