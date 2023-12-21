#ifndef BOARD_INCLUDED
#define BOARD_INCLUDED

#include "globals.h"

class Game;
class BoardImpl;

class Board
{
  public:
    Board(const Game& g);
    ~Board();
    void clear();
    void block();
    void unblock();
    bool placeShip(Point topOrLeft, int shipId, Direction dir);
    bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
    void display(bool shotsOnly) const;
    bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
    bool allShipsDestroyed() const;
      // We prevent a Board object from being copied or assigned
    Board(const Board&) = delete;
    Board& operator=(const Board&) = delete;

  private:
    BoardImpl* m_impl;
};

// Helper
bool checkValid(int r, int length);
bool checkValidPoint(Point p, int nRow, int nCol);

#endif // BOARD_INCLUDED
