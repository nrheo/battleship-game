#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

using namespace std;

class GameImpl
{
  public:
    int nRows;
    int nCols;
    int lenShips;
    string shipNames[10];
    char shipSymbols[10];
    int shipLengths[10];
    
    //
    GameImpl(int nRows, int nCols);
    int rows() const;
    int cols() const;
    bool isValid(Point p) const;
    Point randomPoint() const;
    bool addShip(int length, char symbol, string name);
    int nShips() const;
    int shipLength(int shipId) const;
    char shipSymbol(int shipId) const;
    string shipName(int shipId) const;
    Player* play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause);
};

void waitForEnter()
{
    cout << "Press enter to continue: ";
    cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols)
{
    // This compiles but may not be correct
    this->nRows = nRows;
    this->nCols = nCols;
    lenShips = 0;
    for(int i=0; i<10; i++) {
        shipNames[i] = "\0";
        shipSymbols[i] = ' ';
        shipLengths[i] = 0;
    }
}

int GameImpl::rows() const
{
    return nRows;  // This compiles but may not be correct
}

int GameImpl::cols() const
{
    return nCols;  // This compiles but may not be correct
}

bool GameImpl::isValid(Point p) const
{
    return p.r >= 0  &&  p.r < rows()  &&  p.c >= 0  &&  p.c < cols();
}

Point GameImpl::randomPoint() const
{
    return Point(randInt(rows()), randInt(cols()));
}

bool GameImpl::addShip(int length, char symbol, string name)
{
    shipNames[lenShips] = name;
    shipSymbols[lenShips] = symbol;
    shipLengths[lenShips] = length;
    lenShips++;
    return true;  // This compiles but may not be correct
}

int GameImpl::nShips() const
{
    return lenShips;  // This compiles but may not be correct
}

int GameImpl::shipLength(int shipId) const
{
    return shipLengths[shipId];  // This compiles but may not be correct
}

char GameImpl::shipSymbol(int shipId) const
{
    return shipSymbols[shipId];  // This compiles but may not be correct
}

string GameImpl::shipName(int shipId) const
{
    return shipNames[shipId];  // This compiles but may not be correct
}

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause)
{
    if(!p1->placeShips(b1)){
        cout << "Place Ships failed. Please try again." << endl;
        return nullptr;
    }
    if(!p2->placeShips(b2)){
        cout << "Place Ships failed. Please try again." << endl;
        return nullptr;
    }
    
    Point p;
    bool shotHit;
    bool shipDestroyed;
    int shipId;
    bool validShot;
    int winner = 0;
    int round = 1;
    while(true) {
        // p1 turn
//        cout << endl << "Player 1 " << endl;
        
        cout << endl << "(Round "<< round << ") Player 1 turn" << endl;
        if(!p1->isHuman()) {
            b2.display(false);
        } else {
            b2.display(true);
        }
        
        p = p1->recommendAttack();
        cout << "Player 1 attack : (" << p.r << ", " << p.c << ")" << endl;
        validShot = b2.attack(p, shotHit, shipDestroyed, shipId);
        p1->recordAttackResult(p, validShot, shotHit, shipDestroyed, shipId);
        p2->recordAttackByOpponent(p);
        
        if(!p1->isHuman()) {
            b2.display(false);
        } else {
            b2.display(true);
        }
        
        
        if(b2.allShipsDestroyed()) {
            winner = 1;
            break;
        }
        
        cout << endl << "(Round "<< round << ") Player 2 turn" << endl;
//        cout << endl << "Player 2 " << endl;
        if(!p2->isHuman()) {
            b1.display(false);
        } else {
            b1.display(true);
        }
        
        p = p2->recommendAttack();
        cout << "Player 2 attack : (" << p.r << ", " << p.c << ")" << endl;
        validShot = b1.attack(p, shotHit, shipDestroyed, shipId);
        p2->recordAttackResult(p, validShot, shotHit, shipDestroyed, shipId);
        p1->recordAttackByOpponent(p);
        
        if(!p2->isHuman()) {
            b1.display(false);
        } else {
            b1.display(true);
        }
        
        if(b1.allShipsDestroyed()) {
            winner = 2;
            break;
        }
        if(shouldPause){
            cout << "Press enter to continue: ";
            cin.ignore();
        }
        round++;
    }
    
    if(winner == 1) {
        if(p1->isHuman()){
            b2.display(false);
        }
        return p1;
    } else {
        if(p2->isHuman()){
            b1.display(false);
        }
        return p2;
    }
//    cout << "winner : " << winner << endl;
    
    
}

//******************** Game functions *******************************

// These functions for the most part simply delegate to GameImpl's functions.
// You probably don't want to change any of the code from this point down.

Game::Game(int nRows, int nCols)
{
    if (nRows < 1  ||  nRows > MAXROWS)
    {
        cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
        exit(1);
    }
    if (nCols < 1  ||  nCols > MAXCOLS)
    {
        cout << "Number of columns must be >= 1 and <= " << MAXCOLS << endl;
        exit(1);
    }
    m_impl = new GameImpl(nRows, nCols);
}

Game::~Game()
{
    delete m_impl;
}

int Game::rows() const
{
    return m_impl->rows();
}

int Game::cols() const
{
    return m_impl->cols();
}

bool Game::isValid(Point p) const
{
    return m_impl->isValid(p);
}

Point Game::randomPoint() const
{
    return m_impl->randomPoint();
}

bool Game::addShip(int length, char symbol, string name)
{
    if (length < 1)
    {
        cout << "Bad ship length " << length << "; it must be >= 1" << endl;
        return false;
    }
    if (length > rows()  &&  length > cols())
    {
        cout << "Bad ship length " << length << "; it won't fit on the board"
             << endl;
        return false;
    }
    if (!isascii(symbol)  ||  !isprint(symbol))
    {
        cout << "Unprintable character with decimal value " << symbol
             << " must not be used as a ship symbol" << endl;
        return false;
    }
    if (symbol == 'X'  ||  symbol == '.'  ||  symbol == 'o')
    {
        cout << "Character " << symbol << " must not be used as a ship symbol"
             << endl;
        return false;
    }
    int totalOfLengths = 0;
    for (int s = 0; s < nShips(); s++)
    {
        totalOfLengths += shipLength(s);
        if (shipSymbol(s) == symbol)
        {
            cout << "Ship symbol " << symbol
                 << " must not be used for more than one ship" << endl;
            return false;
        }
    }
    if (totalOfLengths + length > rows() * cols())
    {
        cout << "Board is too small to fit all ships" << endl;
        return false;
    }
    return m_impl->addShip(length, symbol, name);
}

int Game::nShips() const
{
    return m_impl->nShips();
}

int Game::shipLength(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipName(shipId);
}

Player* Game::play(Player* p1, Player* p2, bool shouldPause)
{
    if (p1 == nullptr  ||  p2 == nullptr  ||  nShips() == 0)
        return nullptr;
    Board b1(*this);
    Board b2(*this);
    return m_impl->play(p1, p2, b1, b2, shouldPause);
}

