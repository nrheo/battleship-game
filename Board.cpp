#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>

using namespace std;

class BoardImpl
{
  public:
    BoardImpl(const Game& g);
    void clear();
    void block();
    void unblock();
    bool placeShip(Point topOrLeft, int shipId, Direction dir);
    bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
    void display(bool shotsOnly) const;
    bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
    bool allShipsDestroyed() const;

  private:
      // TODO:  Decide what private members you need.  Here's one that's likely
      //        to be useful:
    Point blockPoint;
    bool isBlock;
    char board[10][10];
    const Game& m_game;
    int ships[5];
    int numOfShips;
};



BoardImpl::BoardImpl(const Game& g)
 : m_game(g)
{
    // This compiles, but may not be correct
    clear();
    
}

void BoardImpl::clear()
{
    // This compiles, but may not be correct
    for(int i=0; i<10; i++) {
        for(int j=0; j<10; j++) {
            board[i][j] = ' ';
        }
    }
    isBlock = false;
    for(int i=0; i<5; i++) {
        ships[i] = -1;
    }
    numOfShips = 0;
}

void BoardImpl::block()
{
    // TODO:  Replace this with code to block half of the cells on the board
    int num = (m_game.rows() * m_game.cols()) / 2;
    int r,c;
    int count = 0;
    while(true) {
        if(count == num) {
            break;
        }
        r = randInt(m_game.rows());
        c = randInt(m_game.cols());
        if(board[r][c] != '#') {
            board[r][c] = '#';
            count++;
        }
    }
}

void BoardImpl::unblock()
{
    // TODO:  Replace this with code to unblock all blocked cells
    for(int i=0; i < m_game.rows(); i++) {
        for(int j=0; j<m_game.cols(); j++) {
            if(board[i][j] == '#') {
                board[i][j] = ' ';
            }
        }
    }
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    
    if (shipId < 0 || shipId >= m_game.nShips()) {
        return false;
    }
    // check board
    if(dir == HORIZONTAL) {
        // hori
        for(int i=0; i< m_game.shipLength(shipId); i++) {
            if(topOrLeft.c+i >= m_game.cols()) {
                return false;
            }
            if(board[topOrLeft.r][topOrLeft.c+i] != ' ') {
                return false;
            } else {
                if(isBlock) {
                    if(blockPoint.r == topOrLeft.r && blockPoint.c == topOrLeft.c+i) {
                        return false;
                    }
                }
            }
        }
    } else {
        // verti
        for(int i=0; i< m_game.shipLength(shipId); i++) {
            if(topOrLeft.r+i >= m_game.rows()) {
                return false;
            }
            if(board[topOrLeft.r+i][topOrLeft.c] != ' ') {
                return false;
            } else {
                if(isBlock) {
                    if(blockPoint.r == topOrLeft.r+i && blockPoint.c == topOrLeft.c) {
                        return false;
                    }
                }
            }
        }
    }
    
    // place
    if(dir == HORIZONTAL) {
        for(int i=0; i< m_game.shipLength(shipId); i++) {
            board[topOrLeft.r][topOrLeft.c+i] = m_game.shipSymbol(shipId);
        }
    }
    else {
        for(int i=0; i< m_game.shipLength(shipId); i++) {
            board[topOrLeft.r+i][topOrLeft.c] = m_game.shipSymbol(shipId);
        }
    }
    
    ships[numOfShips] = shipId;
    numOfShips++;
    
    return true; // This compiles, but may not be correct
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    if (shipId < 0 || shipId >= m_game.nShips()) {
            return false;
    }
    // check
    if(dir == HORIZONTAL) {
        // hori
        for(int i=0; i< m_game.shipLength(shipId); i++) {
            if(topOrLeft.c+i >= m_game.cols()) {
                return false;
            }
            if(board[topOrLeft.r][topOrLeft.c+i] != m_game.shipSymbol(shipId)) {
                return false;
            }
        }
    } else {
        // verti
        for(int i=0; i< m_game.shipLength(shipId); i++) {
            if(topOrLeft.r+i >= m_game.rows()) {
                return false;
            }
            if(board[topOrLeft.r+i][topOrLeft.c] != m_game.shipSymbol(shipId)) {
                return false;
            }
        }
    }
    // unplace
    if(dir == HORIZONTAL) {
        // hori
        for(int i=0; i< m_game.shipLength(shipId); i++) {
            board[topOrLeft.r][topOrLeft.c+i] = ' ';
        }
    } else {
        // verti
        for(int i=0; i< m_game.shipLength(shipId); i++) {
            board[topOrLeft.r+i][topOrLeft.c] = ' ';
        }
    }
    
    for(int i=0; i< numOfShips; i++) {
        if(ships[i] == shipId) {
            ships[i] = -1;
        }
    }
    numOfShips--;
    int emptyIndex = -1;
    for(int i=0; i< 5; i++) {
        if(ships[i] == -1) {
            emptyIndex = i;
        } else {
            if(emptyIndex > 0) {
                ships[emptyIndex] = ships[i];
                ships[i] = -1;
            }
        }
    }
    
    return true; // This compiles, but may not be correct
}

void BoardImpl::display(bool shotsOnly) const
{
    // This compiles, but may not be correct
    cout << "  ";
    for (int i=0; i<m_game.cols(); i++) {
        cout << i;
    }
    cout << endl;
    for (int i=0; i<m_game.rows(); i++) {
        cout << i << " ";
        for (int j=0; j<m_game.cols(); j++) {
            if(shotsOnly) {
                if(board[i][j] == 'o' || board[i][j] == 'X') {
                    cout << board[i][j];
                } else {
                    cout << ".";
                }
            } else {
                if (board[i][j] == ' ') {
                    cout << ".";
                } else {
                    cout << board[i][j];
                }
            }
            
        }
        cout << endl;
    }
}

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    if(!checkValidPoint(p, m_game.rows(), m_game.cols())){
        return false;
    }
    if(board[p.r][p.c] == 'X' || board[p.r][p.c] == 'o') {
        return false;
    }
    if(board[p.r][p.c] == ' ') {
        board[p.r][p.c] = 'o';
        shotHit = false;
        shipDestroyed = false;
        return true;
    } else {
        shotHit = true;
        char symbol = board[p.r][p.c];
        board[p.r][p.c] = 'X';
        
        // check Destroyed or not
        int count = 0;
        for(int i=0; i<m_game.rows(); i++) {
            for(int j=0; j<m_game.cols(); j++) {
                if(board[i][j] == symbol) {
                    count++;
                }
            }
        }
        
        if(count < 1) {
            shipDestroyed = true;
            for(int i=0; i< numOfShips; i++) {
                if(symbol == m_game.shipSymbol(ships[i])){
                    shipId = ships[i];
                }
            }
        } else {
            shipDestroyed = false;
        }
    }
    
    return true; // This compiles, but may not be correct
}

bool BoardImpl::allShipsDestroyed() const
{
    int count = 0;
    for(int i=0; i<m_game.rows(); i++) {
        for(int j=0; j<m_game.cols(); j++) {
            if(!(board[i][j] == ' ' || board[i][j] == 'X' || board[i][j] == 'o')) {
                count++;
            }
        }
    }
    if(count > 0) {
        return false;
    }
    return true; // This compiles, but may not be correct
}

//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.

Board::Board(const Game& g)
{
    m_impl = new BoardImpl(g);
}

Board::~Board()
{
    delete m_impl;
}

void Board::clear()
{
    m_impl->clear();
}

void Board::block()
{
    return m_impl->block();
}

void Board::unblock()
{
    return m_impl->unblock();
}

bool Board::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->placeShip(topOrLeft, shipId, dir);
}

bool Board::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->unplaceShip(topOrLeft, shipId, dir);
}

void Board::display(bool shotsOnly) const
{
    m_impl->display(shotsOnly);
}

bool Board::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
    return m_impl->allShipsDestroyed();
}


bool checkValid(int r, int length) {
    if(r < 0 || r >= length) {
        return false;
    }
    return true;
}

bool checkValidPoint(Point p, int nRow, int nCol) {
//    cout << "checkValidPoint : (" << p.r << ", " << p.c << ")" << endl;
    return checkValid(p.r, nRow) && checkValid(p.c, nCol);
}

