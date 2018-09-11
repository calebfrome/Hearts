#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTextStream>
#include <iostream>
#include <QTime>
#include <QMessageBox>
#include <QThread>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //std::cout << "  ____  ____" << endl;
    //std::cout << " /   \\\\/   \\\\   __    __  _____     __     _____  ________   _______" << endl;
    //std::cout << "|           ||  ||    || ||        //\\    ||   \\     ||     //" << endl;
    //std::cout << " \\         //   ||____|| ||____   //__\\   ||___/     ||     \\\\___" << endl;
    //std::cout << "   \\     //     ||    || ||      //    \\  ||   \\     ||          \\\\" << endl;
    //std::cout << "     \\ //       ||    || ||____ //      \\ ||    \\    ||   _______//    (c)2016 Caleb Frome\n" << endl;

    connect(ui->action_New_Game,SIGNAL(triggered()),this,SLOT(playGame()));
    //connect(ui->listWidget_myHand,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(card_doubleClicked()));
    connect(ui->listWidget_myHand,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(card_clicked()));

    QIcon clubIcon(":/icons/club.png");
    QIcon diamondIcon(":/icons/diamond.png");
    QIcon spadeIcon(":/icons/spade.png");
    QIcon heartIcon(":/icons/heart.png");
    iconArray[0] = clubIcon;
    iconArray[1] = diamondIcon;
    iconArray[2] = spadeIcon;
    iconArray[3] = heartIcon;

    ui->label_p0->setText(player0);
    ui->label_p1->setText(player1);
    ui->label_p2->setText(player2);
    ui->label_p3->setText(player3);

    qsrand((uint) QTime::currentTime().msec());
    //QMetaEnum metaEnum = QMetaEnum::fromType<MainWindow::PassDirection>();
    noPassRoundEnabled=false;

    //playGame(); //TODO not sure if this should be enabled, i.e. whether a game should start automatically
}

MainWindow::~MainWindow()
{
    delete ui;
}

//return a random integer between the low and high bounds (inclusive?)
int MainWindow::randomInt(int low, int high) {
    return qrand() % ((high + 1) - low) + low;
}

//setup variables and ui for a new game
void MainWindow::newGame() {

    //setup variables//

    //cards played/unplayed
    for(int i=0;i<4;i++) {
        for(int j=0;j<13;j++) {
                cardsPlayed[i][j] = false;
        }
        unplayedSuitTotals[i] = 13;
    }

    //initialize player info structs
    pInfo[0].hasQueen = 0;
    pInfo[0].shooting = false;
    pInfo[1].hasQueen = 0;
    pInfo[1].shooting = false;
    pInfo[2].hasQueen = 0;
    pInfo[2].shooting = false;
    pInfo[3].hasQueen = 0;
    pInfo[3].shooting = false;

    SuitStatus emptyStatus;
    emptyStatus.numCards = 0;
    emptyStatus.numVoid = 0;
    emptyStatus.suit = -1;
    emptyStatus.S_LEAD = false;
    emptyStatus.D_ALLVOID = false;
    emptyStatus.D_HIGH = false;
    emptyStatus.D_ALLHIGH = false;
    emptyStatus.D_AKS = false;
    emptyStatus.D_QS = false;

    for(int i=0;i<4;i++) {
        for(int j=0;j<4;j++) {
            suitStatus[i][j] = emptyStatus;
            knownVoid[i][j] = false;
        }
    }


    numRows=1;
    for(int i=0;i<4;i++) {
        handPoints[i]=0;
        scores[i]=0;
    }
    passDirection=PD_LEFT;
    setNoPassRound();

    /***setup ui***/

    //score widget
    ui->tableWidget_scores->setEnabled(true);
    ui->tableWidget_scores->setRowCount(numRows);
    ui->tableWidget_scores->setColumnCount(4);
    ui->tableWidget_scores->setMouseTracking(false);
    QStringList playerNames;
    playerNames.insert(0,player0);
    playerNames.insert(1,player1);
    playerNames.insert(2,player2);
    playerNames.insert(3,player3);
    ui->tableWidget_scores->setHorizontalHeaderLabels(playerNames);
    ui->tableWidget_scores->setCornerButtonEnabled(false);
    ui->tableWidget_scores->horizontalHeader()->setSectionsClickable(false);
    ui->tableWidget_scores->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidget_scores->verticalHeader()->setSectionsClickable(false);
    ui->tableWidget_scores->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidget_scores->setColumnWidth(0,60);
    ui->tableWidget_scores->setColumnWidth(1,60);
    ui->tableWidget_scores->setColumnWidth(2,60);
    ui->tableWidget_scores->setColumnWidth(3,60);
    QTableWidgetItem *item;
    for(int row = 0; row < numRows; row++) {
        for(int column = 0; column < 4; column ++) {
            item = new QTableWidgetItem();
            item->setFlags(Qt::NoItemFlags);
            ui->tableWidget_scores->setItem(row,column,item);
        }
    }
    ui->listWidget_myHand->clear();
    ui->listWidget_p0->clear();
    ui->listWidget_p1->clear();
    ui->listWidget_p2->clear();
    ui->listWidget_p3->clear();
}

//play a game of hearts
void MainWindow::playGame() {
    newGame();

    //loop is the play of one hand
    while(scores[0]<100 && scores[1]<100 && scores[2]<100 && scores[3]<100) {

        //initialize variables
        handSize=13;

        createDeck();
        dealCards();

        //pass cards
        bool passEnable=true; //debug
        if(passEnable) {
            int direction = (int)passDirection;
            //cout << "pass direction is " << passDirection << endl; //metaEnum.keyToValue(direction)
            passCards();
            direction++;
            if(direction>roundsPerPassCycle)
                direction=0;
            passDirection = (PassDirection)direction;
        }

        //debug
        showAllHands();

        lead = getCardLocation(2,0);

        //loop plays one trick
        for(int trick=1;trick<=13;trick++) {
            playTrick();
        }
        updateScores();

    }
    if(scores[0]<=scores[1] && scores[0]<=scores[2] && scores[0]<=scores[3]) {
        ui->label_statusMessage->setText("Congratulations!\nYou're smarter than my program!\n\nYou're on a hot streak - play again?");
    }
    else ui->label_statusMessage->setText("It seems you've lost. We're not surprised.");
    //updateStats(); //TODO
}

//setup settings for card passing
void MainWindow::setNoPassRound() {
    if(noPassRoundEnabled)
        roundsPerPassCycle=4;
    if(!noPassRoundEnabled)
        roundsPerPassCycle=3;
}

//create a shuffled deck of cards using a swapping algorithm
void MainWindow::createDeck() {
    deck = Hand();

    //create cards and place them in the deck
    for(int suit = 0;suit<4;suit++) {
        for(int rank=2;rank<=14;rank++) {
            Card card(rank,suit);
            deck.addCard(card);
        }
    }

    //use swap algorithm to shuffle the deck
    int random;
    Card temp = Card();
    for(int i=0;i<52;i++) {
        random = randomInt(i,51);
        temp = deck[i];
        deck.set(i,deck[random]);
        deck.set(random,temp);
    }
}

//deal 13 cards to each player
void MainWindow::dealCards() {

    //create hands
    for(int i=0;i<4;i++) {
        hands[i] = Hand();
    }

    //populate hands
    int pos=0;
    for(int i=0;i<13;i++) {
        for(int j=0;j<4;j++) {
            hands[j].addCard(deck[pos++]);
        }
    }

    sortAllHands();

    //setup my hand in the GUI
    ui->listWidget_myHand->clear();
    QListWidgetItem *cardItem;
    for(int i=0;i<13;i++) {
        cardItem = new QListWidgetItem(QIcon(),QString::number(i),0,0);
        ui->listWidget_myHand->addItem(cardItem);
    }

    ui->listWidget_myHand->setWordWrap(true);
    ui->listWidget_myHand->setTextElideMode(Qt::ElideNone);
    ui->listWidget_passQueue->setWordWrap(true);
    ui->listWidget_passQueue->setTextElideMode(Qt::ElideNone);

    QString suitTip;
    for(int i=0;i<13;i++) {
        ui->listWidget_myHand->item(i)->setIcon(iconArray[hands[0][i].getSuitInt()]);
        ui->listWidget_myHand->item(i)->setText(hands[0][i].getRankString());
        suitTip = hands[0][i].getSuitString();
        ui->listWidget_myHand->item(i)->setToolTip(suitTip);
        suitTip = ui->listWidget_myHand->item(i)->toolTip();
        ui->listWidget_myHand->item(i)->setSizeHint(QSize(64,120));
        ui->listWidget_myHand->item(i)->setTextAlignment(Qt::AlignVCenter);
    }
}

//sort each hand by ascending rank within each suit
void MainWindow::sortAllHands() {

    //create tempHands
    Hand tempHands[4];
    for(int i=0;i<4;i++) {
        tempHands[i] = Hand();
    }

    //sort cards by rank
    for(int rank=2;rank<=14;rank++) {
        for(int pos=0;pos<handSize;pos++) {
            for(int hand=0;hand<4;hand++) {
                if(hands[hand][pos].getRank() == rank) {
                    tempHands[hand].addCard(hands[hand][pos]);
                }
            }
        }
    }

    //clear hands before adding cards back
    for(int hand=0;hand<4;hand++) {
        hands[hand].clear();
    }

    //sort cards by suit
    for(int suit=0;suit<4;suit++) {
        for(int pos=0;pos<handSize;pos++) {
            for(int hand=0;hand<4;hand++) {
                if(tempHands[hand][pos].getSuitInt() == suit) {
                    hands[hand].addCard(tempHands[hand][pos]);
                }
            }
        }
    }
}

//use qDebug() to display all hands
void MainWindow::showAllHands() {
    QDebug bug = qDebug();
    for(int i=0;i<4;i++) {
        bug.nospace() << "\nPlayer " << i << "'s hand:" << endl;
        for(int j=0;j<handSize;j++) {
            bug.noquote().space() << hands[i][j].toString();
        }
    }
}

//use qDebug() to display one player's hand
void MainWindow::showHand(int player) {
    qDebug().nospace() << "\nPlayer" << player << "'s hand:";
    QDebug bug = qDebug();
    for(int i=0;i<handSize;i++) {
        bug.noquote() << hands[player][i].toString();
    }
}

//handles the passing of cards at the beginning of each hand
void MainWindow::passCards() {
    //No pass caveat
    if(passDirection == PD_NONE)
        return;

    if(passDirection == PD_LEFT)
        ui->label_statusMessage->setText("Select three cards to pass left.");
    if(passDirection == PD_RIGHT)
        ui->label_statusMessage->setText("Select three cards to pass right.");
    if(passDirection == PD_ACROSS)
        ui->label_statusMessage->setText("Select three cards to pass across.");

    playSelection=false;
    cardsToPass.clear();
    ui->listWidget_passQueue->clear();
    //loop until user chooses three cards to pass
    QEventLoop loop;
    QObject::connect(this, SIGNAL(quitLoop()), &loop, SLOT(quit()), Qt::QueuedConnection);
    loop.exec();

    sort3Ints(myPass1,myPass2,myPass3);
    Card m_1=hands[0][myPass1];
    Card m_2=hands[0][myPass2];
    Card m_3=hands[0][myPass3];
    hands[0].removeThreeCards(m_1,m_2,m_3);

    Card passCards[4][3];

    Card c1_1;
    Card c1_2;
    Card c1_3;
    Card c2_1;
    Card c2_2;
    Card c2_3;
    Card c3_1;
    Card c3_2;
    Card c3_3;

    hands[1].getAIPass(c1_1,c1_2,c1_3);
    hands[2].getAIPass(c2_1,c2_2,c2_3);
    hands[3].getAIPass(c3_1,c3_2,c3_3);

    passCards[0][0] = m_1;
    passCards[0][1] = m_2;
    passCards[0][2] = m_3;
    passCards[1][0] = c1_1;
    passCards[1][1] = c1_2;
    passCards[1][2] = c1_3;
    passCards[2][0] = c2_1;
    passCards[2][1] = c2_2;
    passCards[2][2] = c2_3;
    passCards[3][0] = c3_1;
    passCards[3][1] = c3_2;
    passCards[3][2] = c3_3;

    //pass left
    if(passDirection == PD_LEFT) {
        for(int i=0;i<4;i++) {
            for(int j=0;j<3;j++) {
                hands[i].addCard(passCards[(i+3)%4][j]);
            }
        }
    }

    //pass right or across
    if(passDirection == PD_RIGHT || passDirection == PD_ACROSS) {
        for(int i=0;i<4;i++) {
            for(int j=0;j<3;j++) {
                hands[i].addCard(passCards[(i+passDirection)%4][j]);
            }
        }
    }

    ui->label_statusMessage->setText("You have been passed the following cards:\n" +
                                     hands[0][10].toString() + "\n" +
                                     hands[0][11].toString() + "\n" +
                                     hands[0][12].toString());

    sortAllHands();

    //update hand in GUI
    for(int i=0;i<13;i++) {
        ui->listWidget_myHand->item(i)->setIcon(iconArray[hands[0][i].getSuitInt()]);
        ui->listWidget_myHand->item(i)->setText(hands[0][i].getRankString());
        ui->listWidget_myHand->item(i)->setSizeHint(QSize(64,120));
        ui->listWidget_myHand->item(i)->setTextAlignment(Qt::AlignVCenter);
        ui->listWidget_myHand->item(i)->setSelected(false);
    }
    qApp->processEvents();
    QThread::msleep(1500);
    ui->label_statusMessage->clear();
    playSelection=true;
}

//Sorts three integers, using swapping (ints are passed by reference)
void MainWindow::sort3Ints(int &int1, int &int2, int &int3) {
    if(int1>int2)
        std::swap(int1,int2);
    if(int1>int3)
        std::swap(int1,int3);
    if(int2>int3)
        std::swap(int2,int3);
}

//Manages the play of a single trick
void MainWindow::playTrick() {
    int player = lead;
    int temp = lead;
    for(int i=0;i<4;i++) {
        if((temp+i)>3) temp -= 4;
        trickOrder[i] = temp+i;
    }
    int trickPoints=0;
    sortAllHands();
    //updatePlayerInfo();

    //everyone plays once
    for(int plays=0;plays<4;plays++) {
        updatePlayerInfo(player);
        trickCards.addCard(playCard(player++));
        if(trickCards[plays].getSuitInt() == 3) {
            trickPoints++;
        }
        if(trickCards[plays].isCard(12,2)) {
            trickPoints+=13;
        }
        if(player>3) {
            player = 0;
        }
        qApp->processEvents();
        QThread::sleep(1);
    }
    QThread::sleep(1);

    //determine trick winner and assign points
    int trickWinner = player;
    int leadSuit=trickCards[0].getSuitInt();
    int winningRank = trickCards[0].getRank();

    for(int i=1;i<4;i++) {
        player++;
        if(player>3)
            player=0;
        if(trickCards[i].getSuitInt() == leadSuit && trickCards[i].getRank() > winningRank) {
            winningRank = trickCards[i].getRank();
            trickWinner = player;
        }
    }

    trickCards.clear();
    handPoints[trickWinner] += trickPoints;
    totalHandPoints+=trickPoints;

    ui->listWidget_p0->clear();
    ui->listWidget_p1->clear();
    ui->listWidget_p2->clear();
    ui->listWidget_p3->clear();
    handSize--;
    lead = trickWinner;
}

//I play a card
QListWidgetItem* MainWindow::myTurn(Card &card) {
    QEventLoop loop;
    QObject::connect(this, SIGNAL(quitLoop()), &loop, SLOT(quit()), Qt::QueuedConnection);
    loop.exec();
    card = hands[0][myCard];
    hands[0].removeCard(card);
    return ui->listWidget_myHand->takeItem(myCard);
}

//Click a card to move it to/from the pass queue, or to play it
void MainWindow::card_clicked() {
    QListWidgetItem *handCard = new QListWidgetItem();
    QListWidgetItem *passCard = new QListWidgetItem();
    //if playing cards
    if(playSelection) {
        myCard = ui->listWidget_myHand->currentRow();
        emit quitLoop();
    }
    //if passing cards
    else {
        handCard = ui->listWidget_myHand->currentItem();
        passCard->setText(handCard->text());
        passCard->setIcon(handCard->icon());
        passCard->setToolTip(handCard->toolTip());
        passCard->setSizeHint(QSize(64,30));
        passCard->setTextAlignment(Qt::AlignVCenter);
        //don't allow more than 3 cards to be selected
        if(ui->listWidget_myHand->selectedItems().count() > 3) {
            ui->listWidget_myHand->currentItem()->setSelected(false);
            return;
        }
        //remove the card from the queue if it is already there
        QListWidgetItem *temp;
        for(QListWidgetItem* c: cardsToPass) {
            if((c->text() == passCard->text()) && (c->toolTip() == passCard->toolTip())) {
                qDebug() << "removing card: " << passCard->text().toStdString().c_str() << " of " << passCard->toolTip();
                cardsToPass.removeOne(c);

                for(int card=0; card<ui->listWidget_passQueue->count(); card++) {
                //for(QListWidgetItem i: ui->listWidget_passQueue->)
                    temp = ui->listWidget_passQueue->item(card);
                    if((temp->text() == passCard->text()) && (temp->toolTip() == passCard->toolTip())) {
                        delete ui->listWidget_passQueue->takeItem(card);
                        //ui->listWidget_passQueue->item(0);
                        break;
                    }
                }
                ui->passCardsButton->setEnabled(cardsToPass.size() == 3);
                qApp->processEvents();
                qDebug() << "Pass queue size decreased to: " << ui->listWidget_passQueue->count();
                return;
            }
        }
        //add the card to the queue if it is not already there
        qDebug() << "adding card: " << handCard->text().toStdString().c_str() << " of " << passCard->toolTip();
        cardsToPass.append(handCard);
        ui->listWidget_passQueue->addItem(passCard);
        ui->passCardsButton->setEnabled(cardsToPass.size() == 3);
        qApp->processEvents();
        qDebug() << "Pass queue size increased to: " << ui->listWidget_passQueue->count();
    }
}

//Commit queued cards to the pass
void MainWindow::on_passCardsButton_clicked() {
    myPass1 = ui->listWidget_myHand->row(cardsToPass.at(0));
    myPass2 = ui->listWidget_myHand->row(cardsToPass.at(1));
    myPass3 = ui->listWidget_myHand->row(cardsToPass.at(2));
    ui->listWidget_passQueue->clear();
    ui->passCardsButton->setEnabled(false);
    emit quitLoop();
}

//INWORK Logic which determines the card a computer plays on its turn
Card MainWindow::playCard(int player) {
    int choice = -1; //impossible value: no choice has been made as long as choice is -1
    Card cardChoice;

    //I play a card
    if(player==0) {
        QListWidgetItem *i;
        i = myTurn(cardChoice);
        i->setSizeHint(QSize(75,55));
        i->setTextAlignment(Qt::AlignCenter);
        ui->listWidget_p0->addItem(i);
        qApp->processEvents();
        return cardChoice;
    }

    /**Computer plays a card - AI Strategy!**/

    /*NOT SHOOTING
    leading:
    * first trick? 2C
    * else: make a decision based on suitStatus
    -- spades if S_LEAD
    -- clubs if S_LEAD
    -- diamonds if S_LEAD
    -- hearts if S_LEAD && heartsBroken
    -- whichever of clubs/diamonds is closer to void or closer to safe

    not leading:
    * follow suit
    * dumpCard()


    SHOOTING
    *
    *
    */


    //NEW CODE//

    //leading
    if(trickOrder[0] == player) {
        //Lead 2C if you have it
        if(hands[player].contains(2,0)) {
            choice = 0;
        }
        //lead spades if safe
        else if(suitStatus[player][2].S_LEAD) {
            choice = hands[player].highestInSuitPosMax(2,11);
        }
        //lead clubs if safe
        else if(suitStatus[player][0].S_LEAD) {
            choice = hands[player].highestInSuitPos(0);
        }
        //lead diamonds if safe
        else if(suitStatus[player][1].S_LEAD) {
            choice = hands[player].highestInSuitPos(1);
        }
        //lead hearts if safe && heartsBroken
        else if(suitStatus[player][3].S_LEAD && heartsBroken) {
            choice = hands[player].highestInSuitPos(3);
        }
        //lead spades if not void
        else if(suitStatus[player][2].numCards > 0) {
            choice = hands[player].highestInSuitPos(2);
        }
        //lead low card if not void
        else {
            int suitPref[] = {2,0,1,3};
            for(int i:suitPref) {
                if(suitStatus[player][i].numCards > 0) {
                    choice = hands[player].lowestInSuitPos(i);
                    break;
                }
            }
        }
    }
    //not leading
    else {
        int suit = trickCards[0].getSuitInt();
        //INWORK follow suit
        if(suitStatus[player][suit].numCards > 0) {
            bool safeLead = (suitStatus[player][0].S_LEAD || suitStatus[player][1].S_LEAD || suitStatus[player][2].S_LEAD || suitStatus[player][3].S_LEAD);
            int trickPoints = 0;
            int maxTrickCard = 0;
            for(int i=0; i<trickCards.numCards(); i++) {
                if(trickCards[i].getSuitInt() == 3)
                    trickPoints++;
                if(trickCards[i].getSuitInt() == suit)
                    maxTrickCard = qMax(maxTrickCard, trickCards[i].getRank());
            }

            //if there is a safe lead, there are no points on the trick, and the queen is unlikely to fall, play to take
            //TODO: add check for whether queen is likely to fall
            if(safeLead && trickPoints==0) {
                if(suit == 2)
                    choice = hands[player].highestInSuitPosMax(suit, 11);
                else choice = hands[player].highestInSuitPos(suit);
            }
            //otherwise, play highest card that won't take
            //but if must play over, play highest overall card
            else {
                choice = hands[player].highestInSuitPosMax(suit, maxTrickCard);
                if(choice == -1) {
                    //TODO: add check for option
                    choice = hands[player].highestInSuitPos(suit);
                }
            }
        }
        else {
            choice = dumpCard(player,suit);
        }
    }


	//OLD CODE//
    /*if first trick
    if(cHand.size()==13) {
         //play two of clubs if in the lead
        if(player == lead) {
            choice=0;
        }
        //otherwise, play highest club
        if(choice==14) {
            for(int pos=12;pos>=0;pos--) {
                if(cHand.get(pos).getSuit()=="clubs")
                    choice=pos;
            }
        }
        if(choice==14) { //otherwise (if void in clubs)...
            if(handCardPos(cHand,12,"spades")<0) { //if doesn't have the queen
                playThis = handCardPos(cHand,14,"spades"); //dump AS
                if(playThis>0)
                    choice=playThis;
                if(choice==14) {
                    playThis = handCardPos(cHand,13,"spades"); //dump KS
                    if(playThis>0)
                        choice=playThis;
                }
                if(choice==14) { //dump a spade if has only one, and diamonds are safe
                    if(numInSuit(cHand,"spades")==1 && lowestInSuitRank(cHand,"diamonds")<4) {
                        choice=handCardPos(cHand,highestInSuitRank(cHand,"spades"),"spades");
                    }
                }
                if(choice==14) { //dump highest diamond
                    choice=handCardPos(cHand,highestInSuitRank(cHand,"diamonds"),"diamonds");
                }
            }
        }
    }

    //if not first trick
    else {
        //if in the lead
        if(leadCard.getSuit()=="lead") {
            //play lowest spade
            for(int card=0;card<handSize;card++) {
                if(cHand.get(card).getSuit().equals("spades")) {
                    choice=card;
                    card=handSize;
                }
            }
            //else play lowest diamond
            if(choice==14) {
                for(int card=0;card<handSize;card++) {
                    if(cHand.get(card).getSuit().equals("diamonds")) {
                        choice=card;
                        card=handSize;
                    }
                }
            }
            //else play from front
            if(choice==14) {
                choice=0;
            }

        }

        //if following
        if(leadCard.getSuit()!="lead") {
            //attempt to follow suit
            for(int card=0;card<handSize;card++) {
                if(cHand.get(card).getSuit().equals(leadCard.getSuit())) {
                    choice=card;
                    card=handSize; //exit loop
                }
            }
            //if choice hasn't been made (choice still = 14), play high spades
            if(choice==14) {
                int card=0;
                for(Card cH:cHand) {
                    if(cH.toString().equals("12 of spades"))
                        choice=card;
                    if(cH.toString().equals("14 of spades"))
                        choice=card;
                    if(cH.toString().equals("13 of spades"))
                        choice=card;
                    card++;
                }
            }
            //else play from back of hand
            if(choice==14) {
                choice=handSize-1;
            }
        }
    }
    cardChoice = getCardAtPos(cHand,choice);
    */

    //temp: play top down
    //choice = handSize - 1;
    //

    //get card from hand
    cardChoice = hands[player][choice];
    //set card as played
    cardsPlayed[cardChoice.getSuitInt()][cardChoice.getRank()-2] = true;

    QListWidgetItem *i = new QListWidgetItem();
    i->setIcon(iconArray[cardChoice.getSuitInt()]);
    i->setText(cardChoice.getRankString());
    i->setSizeHint(QSize(75,55));
    i->setTextAlignment(Qt::AlignCenter);
    switch(player) {
    case 1:
        ui->listWidget_p1->addItem(i);
        break;
    case 2:
        ui->listWidget_p2->addItem(i);
        break;
    case 3:
        ui->listWidget_p3->addItem(i);
        break;
    }
    qApp->processEvents();

    return hands[player].removeCard(cardChoice);
}

//update each PlayerInfo data structure
void MainWindow::updatePlayerInfo(int player) {

    showHand(player);
    qDebug() << "\nSuitStatus for player " << player << ":" << endl;
    for(int i=0;i<4;i++) {
        suitStatus[player][i] = getSuitStatus(hands[player],i);
    }

    //assume a player is shooting if that player is the only one to have taken points
    for(int i=0;i<4;i++) {
        pInfo[i].shooting = (handPoints[i]>0 &&
                             handPoints[(i+1)%4]==0 &&
                             handPoints[(i+2)%4]==0 &&
                             handPoints[(i+3)%4]==0);
    }
}

//determine suit status for one suit in one player's hand
MainWindow::SuitStatus MainWindow::getSuitStatus(Hand cHand, int suit) {

    int count=0;
    for(int i=0;i<cHand.numCards();i++) {
        if(cHand.at(i).getSuitInt() == suit) {
            count++;
            //qDebug() << cHand.at(i).getRankString().toStdString().c_str();
        }
    }
    //qDebug() << "\n";

    MainWindow::SuitStatus status;
	status.suit = suit;
    status.numCards = count;

    //set default values
    status.numVoid = 0;
    status.S_LEAD = false;
    status.D_ALLVOID = false;
    status.D_HIGH = false;
    status.D_ALLHIGH = false;
    status.D_AKS = false;
    status.D_QS = false;

    //if void in this suit, none of the other fields matter
    if(status.numCards == 0) return status;

    //**if not void, determine other status fields**//

    //numVoid - number of players known to be void in this suit
    for(int i=0;i<4;i++) {
        if(knownVoid[i][suit]) status.numVoid++;
    }

    //count high, low, and safe cards//

    int highCards = 0; //number of high cards in this suit in this hand
    int lowCards = 0;  //number of low cards in this suit in this hand
    int safeCards = 0; //number of safe cards in this suit in this hand
    int suitCardsUnplayed = unplayedSuitTotals[suit];

    //define a low card as any card in the bottom 25% of unplayed cards in the suit.
    count=0;
    if(suitCardsUnplayed==0) {
        lowCards=0;
    }
    else for(int i=0;i<13;i++) {
        if(!cardsPlayed[suit][i]) {
            if(cHand.contains(i+2,suit)) {
                lowCards++;
            }
            count++;
        }
        if(count >= (0.25*suitCardsUnplayed)) break;
    }

    //define a high card as any card in the top 25% of unplayed cards in the suit.
    //given that there must also be at least one unplayed card below it in rank.
    count=0;
    if(suitCardsUnplayed==0) {
        highCards=0;
    }
    else for(int i=0;i<13;i++) {
        if(!cardsPlayed[suit][i]) {
            count++;
            if(count < (0.75*suitCardsUnplayed)) continue;
            if(cHand.contains(i+2,suit) && i>0) {
                highCards++;
            }
        }
    }

    //define a safe card as:
    // (n=0): <10+ rem> any card; <6+ rem> any card <= 9 //inwork
    // (n>0): any card with fewer than 3-n unplayed cards below it in its suit.
    //where n is the total number of players void in this suit.
    if(suitCardsUnplayed==0) {
        safeCards=0;
    }
    else if(status.numVoid==0) {
        if(suitCardsUnplayed >= 10) {
            safeCards = status.numCards;
        }
        else if(suitCardsUnplayed >= 6) {
            for(int i=0;i<status.numCards;i++) {
                if(cHand.at(i).getRank() <= 9) safeCards++;
            }
        }
    }
    else {
        bool flag = false;
        int index=0;

        while(!flag) {
            for(int i=0;i<status.numCards;i++) {
                count=0;
                while(!flag) {
                    if(!cardsPlayed[suit][index++]) {
                        if(cHand.at(i).getRank()==index+2) {
                            flag=true;
                        }
                        else count++;
                    }
                }
                if(count < 3-status.numVoid) {
                    safeCards++;
                }
            }
        }
    }
	
    //S_LEAD - safe to lead this suit
    if(safeCards > 0) status.S_LEAD = true;

    //D_ALLVOID - everyone else is void in this suit
    if(status.numCards == unplayedSuitTotals[suit]) {
        status.D_ALLVOID = true;
    }

    //D_HIGH - contains high cards with coverage that may not last
    if(highCards > 0) status.D_HIGH = true;

    //D_ALLHIGH - contains high cards with insufficient coverage underneath
    if(status.numCards == highCards) status.D_ALLHIGH = true;
	
    //D_QS and D_AKS - contains QS/KS/AS with insufficient coverage
	if(suit == 2) {
        //contains QS with insufficient other spades to avoid taking
        if(cHand.contains(12,2) &&
          ((status.numCards*2)+handSize-10) >= unplayedSuitTotals[2]) {
			status.D_QS = true;
        }
        //contains AS/KS with insufficient coverage to avoid taking, and the queen is unplayed and in another hand
        if(!cardsPlayed[2][12] && !cHand.contains(12,2)) {
            if((cHand.contains(13,2) || cHand.contains(14,2)) &&
              ((status.numCards*2)+handSize-10) >= unplayedSuitTotals[2]) {
                status.D_AKS = true;
            }
        }
	}

    //debug
    QDebug bug = qDebug();
    bug << "Suit:" << status.suit << endl;
    bug << status.numCards << "cards," ;
    bug << status.numVoid << "players void in this suit,";
    bug << "S_LEAD:" << status.S_LEAD;
    bug << ", D_ALLVOID:" << status.D_ALLVOID;
    bug << ", D_HIGH:" << status.D_HIGH;
    bug << ", D_ALLHIGH:" << status.D_ALLHIGH;
    bug << ", D_AKS:" << status.D_AKS;
    bug << ", D_QS:" << status.D_QS;
	
	return status;
}

//logic for computer to dump a card (returns the hand position of the card)
//goal: stop a shoot, OR improve hand by dumping a dangerous card, or working towards a void
//TODO: prevent points dump on first trick
int MainWindow::dumpCard(int player, int leadSuit) {
    qDebug() << "Player " << player << " is dumping a card" << endl;

    //hand index of card to be dumped
    int dumpCard = -1;

    //player shooting (-1 if no one is)
    int pShooting = playerShooting(5);

    //if a player is shooting
    if(pShooting>0) {
        //determine whether the shooter has played
        bool shooterPlayed=false;
        for(int i=0;i<4;i++) {
            if(trickOrder[i] == pShooting) {
                shooterPlayed = true;
                break;
            }
            else if(trickOrder[i] == player) {
                break;
            }
        }

        //count unplayed cards in leading suit higher than card currently winning
        int possWinners = 0;
        int curWinningRank = trickCards[(4+trickTaker-trickOrder[0])%4].getRank();
        for(int i=0;i<13;i++) {
            if(!cardsPlayed[leadSuit][i] && (i+2) > curWinningRank) {
                possWinners++;
            }
        }

        //if the shooter has played, but is not winning, OR
        //if the shooter has not played, but cannot have a card able to beat the currently winning card
        //then dump points if possible
        if((shooterPlayed && pShooting!=trickTaker) || (possWinners == 0)) {
            //if this player has QS and at least one heart
            if(suitStatus[player][2].D_QS && (suitStatus[player][3].numCards > 0)) {
                //if spades is safe AND hearts is dangerous, dump the worst heart
                if((!suitStatus[player][2].D_QS) &&
                   ((suitStatus[player][3].numCards > 0) &&
                    !suitStatus[player][3].S_LEAD))
                {
                    dumpCard = hands[player].highestInSuitPos(3);
                }
                //otherwise, dump the queen
                else {
                    dumpCard = hands[player].indexOfCard(12,2);
                }
            }
            //if QS only, dump queen
            else if(suitStatus[player][2].D_QS) {
                dumpCard = hands[player].indexOfCard(12,2);
            }
            //if hearts only, dump highest heart
            else if(suitStatus[player][3].numCards > 0) {
                dumpCard = hands[player].highestInSuitPos(3);
            }
        }
        //dump anything, but keep high cards
        //TODO: improve later
        else {
            //preferred order of suits to dump from
            int prefOrder[] = {3,1,0,2};
            //attempt to dump second-highest card in a suit
            for(int i:prefOrder) {
                if(hands[player].numInSuit(i)>1) {
                    bool exit = false;
                    int rank = hands[player].highestInSuitRank(i);
                    while(!exit) {
                        if(hands[player].contains(--rank,i)) {
                            exit = true;
                        }
                    }
                    dumpCard = hands[player].indexOfCard(rank,i);
                }
            }
            //if no suit has more than one card, dump in the order C,D,S,H (0,1,2,3)
            for(int i=0;i<4;i++) {
                if(hands[player].numInSuit(i)>0) {
                    dumpCard = hands[player].highestInSuitPos(i);
                    break;
                }
            }
        }
    }
    //if no one is shooting, dump a dangerous card or work toward a void
    else {
        //if contains QS unsafe, dump it
        if(suitStatus[player][3].D_QS) {
            dumpCard = hands[player].indexOfCard(12,2);
        }
        //if contains AKS unsafe, dump one
        else if(suitStatus[player][3].D_AKS) {
            if(hands[player].contains(14,2)) {
                dumpCard = hands[player].indexOfCard(14,2);
            }
            else dumpCard = hands[player].indexOfCard(13,2);
        }
        //if hearts are particularly dangerous, dump the highest
        else if((suitStatus[player][3].numCards > 0) && (!suitStatus[player][3].S_LEAD)) {
            dumpCard = hands[player].highestInSuitPos(3);
        }
        //if you have the queen, dump it
        else if(suitStatus[player][2].D_QS) {
            dumpCard = hands[player].indexOfCard(12,2);
        }
        //if you have AKS, dump one
        else if(hands[player].contains(14,2)) {
            dumpCard = hands[player].indexOfCard(13,2);
        }
        else if(hands[player].contains(13,2)) {
            dumpCard = hands[player].indexOfCard(14,2);
        }
        //if you have high hearts only, dump the highest
        else if(suitStatus[player][3].D_ALLHIGH) {
            dumpCard = hands[player].highestInSuitPos(3);
        }
        //if diamonds are all high and unsafe, dump the highest
        else if((suitStatus[player][1].D_ALLHIGH) && (!suitStatus[player][1].S_LEAD)) {
            dumpCard = hands[player].highestInSuitPos(1);
        }
        //if clubs are all high and unsafe, dump the highest
        else if((suitStatus[player][0].D_ALLHIGH) && (!suitStatus[player][0].S_LEAD)) {
            dumpCard = hands[player].highestInSuitPos(0);
        }

        if(dumpCard >= 0) return dumpCard;

        /* The following scenarios are suit-independent. Preference is: H, D, C, S (3, 1, 0, 2)
         * Hearts are given priority because points, spades are least bc often valuable leads.
         * Clubs and diamonds are essentially equal, but I had to pick one. */
        int prefOrder[] = {3,1,0,2};

        //if a non-safe-lead suit contains one, two, or three cards, dump the highest card in that suit
        for(int numCards=1;numCards<=3;numCards++) {
            for(int i:prefOrder) {
                if((suitStatus[player][i].numCards == numCards) && !suitStatus[player][i].S_LEAD) {
                    dumpCard = hands[player].highestInSuitPos(i);
                    return dumpCard;
                }
            }
        }
        //if a suit contains only one card, dump that card
        for(int i:prefOrder) {
            if(suitStatus[player][i].numCards == 1) {
                dumpCard = hands[player].highestInSuitPos(i);
                return dumpCard;
            }
        }
        //if a suit contains exactly two cards and is D_ALLHIGH, dump the highest card
        for(int i:prefOrder) {
            if((suitStatus[player][i].numCards == 2) && suitStatus[player][i].D_ALLHIGH) {
                dumpCard = hands[player].highestInSuitPos(i);
                return dumpCard;
            }
        }
        //if a suit contains exactly two cards, dump the highest
        for(int i:prefOrder) {
            if(suitStatus[player][i].numCards == 2) {
                dumpCard = hands[player].highestInSuitPos(i);
                return dumpCard;
            }
        }
        //if a suit contains exactly three cards and is D_ALLHIGH, dump the highest card
        for(int i:prefOrder) {
            if((suitStatus[player][i].numCards == 3) && suitStatus[player][i].D_ALLHIGH) {
                dumpCard = hands[player].highestInSuitPos(i);
                return dumpCard;
            }
        }
        //dump the highest card in a suit
        for(int i:prefOrder) {
            if(suitStatus[player][i].numCards > 0) {
                dumpCard = hands[player].highestInSuitPos(i);
                return dumpCard;
            }
        }
    }

    //if method gets here, dumpCard should always be >= 0
    if(dumpCard>=0) return dumpCard;

    //dump from right side of hand (should never get here)
    qDebug() << "error in dumpCard logic";
    return handSize-1;
}

//find which hand contains the given card
int MainWindow::getCardLocation(int rank, int suit) {
    Card card(rank,suit);
    for(int i=0;i<4;i++) {
        if(hands[i].indexOfCard(card)>=0)
            return i;
    }
    qDebug() << "ERRMSG: The " << rank << " of " << suit << " was not found in any hand.";
    return -1;
}

//find which hand contains the given card
int MainWindow::getCardLocation(int rank, Card::Suit suit) {
    return getCardLocation(rank,(int)suit);
}

//find which hand contains the given card
int MainWindow::getCardLocation(Card card) {
    return getCardLocation(card.getRank(), card.getSuitInt());
}

//If a player has taken all the points so far, return that player's number
int MainWindow::playerShooting() {
    if(totalHandPoints == 0)
        return -1;
    for(int i=0;i<4;i++) {
        if(totalHandPoints ==  handPoints[i])
            return i;
    }
    return -1;
}

//Return true if it seems a player might be shooting
int MainWindow::playerShooting(int threshold) {
    if(totalHandPoints<threshold)
        return -1;
    return playerShooting();
}

//INWORK -- Needs logic for computers to decide to add vs subtract.
void MainWindow::updateScores() {
    ui->tableWidget_scores->insertRow(numRows-1);
    numRows++;

    QTableWidgetItem *item;
    for(int column = 0; column < 4; column ++) {
        item = new QTableWidgetItem();
        item->setFlags(Qt::NoItemFlags);
        ui->tableWidget_scores->setItem(numRows-2,column,item);
        //item = ui->tableWidget_scores->item(row,column);
        //item->setText(player0);
    }
    setVerticalScoreHeaders();

    //if I shoot the moon
    if(handPoints[0]==26) {
        moonShots++;
        QMessageBox msgBox(QMessageBox::Question,"You shot the moon!","Add or Subtract?",QMessageBox::Yes|QMessageBox::No,this);
        msgBox.setButtonText(QMessageBox::Yes,"Add");
        msgBox.setButtonText(QMessageBox::No,"Subtract");
        msgBox.setDefaultButton(QMessageBox::Yes);
        if(msgBox.exec()==QMessageBox::Yes) {
            handPoints[0]=0;
            handPoints[1]=26;
            handPoints[2]=26;
            handPoints[3]=26;
        }
        else
            handPoints[0] = -26;
    }
    //if computer shoots the moon
    else if(handPoints[1]==26 || handPoints[2]==26 || handPoints[3]==26) {
        moonShotsAllowed++;
        for(int computer=0;computer<4;computer++) {
            if(handPoints[computer]==26) {
                handPoints[computer]-=52;
            }
        }

        //skip this block if computer should subtract:
        for(int i=0;i<4;i++) {
            handPoints[i]+=26;
        }
        //
    }

    for(int i=0;i<4;i++) {
        scores[i]+=handPoints[i];
    }

    ui->tableWidget_scores->item(numRows-2,0)->setText(QString::number(handPoints[0]));
    ui->tableWidget_scores->item(numRows-2,1)->setText(QString::number(handPoints[1]));
    ui->tableWidget_scores->item(numRows-2,2)->setText(QString::number(handPoints[2]));
    ui->tableWidget_scores->item(numRows-2,3)->setText(QString::number(handPoints[3]));

    ui->tableWidget_scores->item(numRows-1,0)->setText(QString::number(scores[0]));
    ui->tableWidget_scores->item(numRows-1,1)->setText(QString::number(scores[1]));
    ui->tableWidget_scores->item(numRows-1,2)->setText(QString::number(scores[2]));
    ui->tableWidget_scores->item(numRows-1,3)->setText(QString::number(scores[3]));

    //reset hand points variables
    for(int i=0;i<4;i++) {
        handPoints[i] = 0;
    }
    totalHandPoints=0;
}

void MainWindow::setVerticalScoreHeaders() {
    QStringList headerLabels;
    for(int row=1;row<numRows;row++) {
        headerLabels.append(QString::number(row));
    }
    headerLabels.append(QString("Total:"));
    ui->tableWidget_scores->setVerticalHeaderLabels(headerLabels);
}

//INWORK - NOT STARTED -- Waiting to implement stats until I build the interface - not adding to command line version
/*
void MainWindow::updateStats() {
    Scanner statsFile = new Scanner(new File("stats.txt"));
    statsFile.nextLine();
    gamesPlayed=Integer.parseInt(statsFile.nextLine());
    //System.out.println(gamesPlayed);
    statsFile.nextLine();
    wins=Integer.parseInt(statsFile.nextLine());
    //System.out.println(wins);
    statsFile.nextLine();
    seconds=Integer.parseInt(statsFile.nextLine());
    statsFile.nextLine();
    thirds=Integer.parseInt(statsFile.nextLine());
    statsFile.nextLine();
    lasts=Integer.parseInt(statsFile.nextLine());
    statsFile.nextLine();
    statsFile.nextLine(); //no need to read in winning percentage
    statsFile.nextLine();
    int moonShotsTemp=Integer.parseInt(statsFile.nextLine());
    statsFile.nextLine();
    int moonShotsAllowedTemp=Integer.parseInt(statsFile.nextLine());
    statsFile.nextLine();
    int queensTakenTemp=Integer.parseInt(statsFile.nextLine());

    gamesPlayed++;
    int[] scores = new int[] {myScore,c1Score,c2Score,c3Score};
    Arrays.sort(scores);
    if(myScore==scores[0]) {
        System.out.println("You won! Congratulations, you sexy beast ;)");
        wins++;
    }
    else {
        System.out.println("You lost! Better luck next time, you nerd!");
        if(myScore==scores[1])
            seconds++;
        else if(myScore==scores[2])
            thirds++;
        else if(myScore==scores[3])
            lasts++;
    }

    winningPercentage=100.0*(double)wins/(double)gamesPlayed;
    moonShots+=moonShotsTemp;
    moonShotsAllowed+=moonShotsAllowedTemp;
    queensTaken+=queensTakenTemp;

    statsFile.close();

    PrintWriter writeStats = new PrintWriter("stats.txt");
    writeStats.println("Games played:");
    writeStats.println(gamesPlayed);
    writeStats.println("Games won:");
    writeStats.println(wins);
    writeStats.println("Second place finishes:");
    writeStats.println(seconds);
    writeStats.println("Third place finishes:");
    writeStats.println(thirds);
    writeStats.println("Fourth place finishes:");
    writeStats.println(lasts);
    writeStats.println("Winning percentage:");
    writeStats.println(winningPercentage);
    writeStats.println("Moon shots:");
    writeStats.println(moonShots);
    writeStats.println("Moon shots allowed:");
    writeStats.println(moonShotsAllowed);
    writeStats.println("Queens taken:");
    writeStats.println(queensTaken);
    writeStats.close();

    Scanner printStats = new Scanner(new File("stats.txt"));
    while(printStats.hasNextLine())
        System.out.println(printStats.nextLine());
    printStats.close();
}
*/
