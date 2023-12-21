#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <string>

using namespace std;

//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
  public:
    AwfulPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
  private:
    Point m_lastCellAttacked;
};

AwfulPlayer::AwfulPlayer(string nm, const Game& g)
 : Player(nm, g), m_lastCellAttacked(0, 0)
{}

bool AwfulPlayer::placeShips(Board& b)
{
      // Clustering ships is bad strategy
    for (int k = 0; k < game().nShips(); k++)
        if ( ! b.placeShip(Point(k,0), k, HORIZONTAL))
            return false;
    return true;
}

Point AwfulPlayer::recommendAttack()
{
    if (m_lastCellAttacked.c > 0)
        m_lastCellAttacked.c--;
    else
    {
        m_lastCellAttacked.c = game().cols() - 1;
        if (m_lastCellAttacked.r > 0)
            m_lastCellAttacked.r--;
        else
            m_lastCellAttacked.r = game().rows() - 1;
    }
    return m_lastCellAttacked;
}

void AwfulPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
                                     bool /* shotHit */, bool /* shipDestroyed */,
                                     int /* shipId */)
{
      // AwfulPlayer completely ignores the result of any attack
}

void AwfulPlayer::recordAttackByOpponent(Point /* p */)
{
      // AwfulPlayer completely ignores what the opponent does
}

//*********************************************************************
//  HumanPlayer
//*********************************************************************

bool getLineWithTwoIntegers(int& r, int& c)
{
    bool result(cin >> r >> c);
    if (!result)
        cin.clear();  // clear error state so can do more input operations
    cin.ignore(10000, '\n');
    return result;
}

bool getLineWithCharacter(char& c)
{
    bool result(cin >> c);
    if (!result)
        cin.clear();  // clear error state so can do more input operations
    cin.ignore(10000, '\n');
    return result;
}


// TODO:  You need to replace this with a real class declaration and
//        implementation.
//typedef AwfulPlayer HumanPlayer;

class HumanPlayer : public AwfulPlayer
{
public:
    HumanPlayer(string nm, const Game& g);
    virtual bool isHuman() const;
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
private:
    Point m_lastCellAttacked;
};

HumanPlayer::HumanPlayer(string nm, const Game& g)
 : AwfulPlayer(nm, g), m_lastCellAttacked(0, 0)
{}


bool HumanPlayer::isHuman() const{
    return true;
}

bool HumanPlayer::placeShips(Board& b)
{
    int r;
    int c;
    char v;
    cout << "Place your ships." << endl;
    for (int k = 0; k < game().nShips(); k++){
        cout << "Choose Vertical or Horizontal(v or h): ";
        getLineWithCharacter(v);
        cout << "Choose topOrLeft position for ship " << k+1 << " : ";
        getLineWithTwoIntegers(r,c);
        if(v == 'v') {
            if ( ! b.placeShip(Point(r,c), k, VERTICAL)){
                return false;
            }
        } else {
            if (!b.placeShip(Point(r,c), k, HORIZONTAL)){
                return false;;
            }
            
        }
        b.display(false);
    }
    return true;
}

Point HumanPlayer::recommendAttack()
{
    cout << "Choose attack position : ";
    getLineWithTwoIntegers(m_lastCellAttacked.r,m_lastCellAttacked.c);
    return m_lastCellAttacked;
}

void HumanPlayer::recordAttackResult(Point p, bool validShot,
                                     bool shotHit, bool shipDestroyed,
                                     int shipId)
{
      // HumanPlayer completely ignores the result of any attack
}

void HumanPlayer::recordAttackByOpponent(Point p)
{
      // HumanPlayer completely ignores what the opponent does
}

//*********************************************************************
//  MediocrePlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.
//typedef AwfulPlayer MediocrePlayer;
// Remember that Mediocre::placeShips(Board& b) must start by calling
// b.block(), and must call b.unblock() just before returning.

class MediocrePlayer : public AwfulPlayer
{
public:
    MediocrePlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
    Point randomChoose();
    Point getNextDirection();
    bool hasShoted(Point p);
    void resetCoordiHistory();
    int chooseRandomCoordi();
    bool isInRange(Point p);
    
    
private:
    Point m_lastCellAttacked;
    
    Point lastShotPoint;
    Point pivotPoint;
    bool lastShotHit;
    bool lastShipDestroyed;
    int recommendState;
    
    Point *shotHistory;
    int shotCount;
    const int coordinate[4][2] = { -1, 0,   // N
                                    0, 1,   // E
                                    1, 0,   // S
                                    0,-1 }; // W
    bool coordiHistory[4];
    int shotCountInSameDirection;
    int currentDirection;
//    int shipId
};

MediocrePlayer::MediocrePlayer(string nm, const Game& g)
 : AwfulPlayer(nm, g), m_lastCellAttacked(0, 0)
{
    lastShotHit= false;
    lastShipDestroyed = false;
    recommendState = 1;
    
    shotHistory = new Point[g.rows()*g.cols()];
    shotCount = 0;
    resetCoordiHistory();
    shotCountInSameDirection = 0;
    
}

bool placeShipOnBoard(int numShip, int count, Board &b, const Game &game) {
    if(numShip == count) {
        return true;
    }
    
    for(int i=0; i < game.rows(); i++) {
        for(int j=0; j<game.cols(); j++) {
            if(b.placeShip(Point(i,j), count, HORIZONTAL)){
                return placeShipOnBoard(numShip, count+1, b, game);
            } else if(b.placeShip(Point(i,j), count, VERTICAL)) {
                return placeShipOnBoard(numShip, count+1, b, game);
            } else {
                b.unplaceShip(Point(i,j), count, HORIZONTAL);
                b.unplaceShip(Point(i,j), count, VERTICAL);
            }
        }
    }
     
    return false;
    
}

bool MediocrePlayer::placeShips(Board& b)
{
    for(int i=0; i<50; i++) {
        b.block();
        if(placeShipOnBoard(game().nShips(), 0, b, game())){
            
            break;
        }
        b.unblock();
    }
    b.unblock();
//    b.display(false);
    
    return true;
}

bool MediocrePlayer::isInRange(Point p){
    return checkValidPoint(p, game().rows(), game().cols());
}

int MediocrePlayer::chooseRandomCoordi(){
    int count =0;
    int coordi = -1;
    
    shotCountInSameDirection = 1;
    
    for(int i=0; i<4; i++) {
        if(coordiHistory[i] == false){
            count++;
            coordi = i;
        }
    }
    
    if(count == 0) {
        return -1; // already visit all direction
    } else if(count == 1) {
        coordiHistory[coordi] = true;
        return coordi;
    } else {
        while(true) {
            coordi = randInt(4);
            if(coordiHistory[coordi] == false) {
                coordiHistory[coordi] = true;
                break;
            }
        }
    }
    return coordi;
}
void MediocrePlayer::resetCoordiHistory(){
    for(int i=0; i<4; i++) {
        coordiHistory[i] = false;
    }
}
bool MediocrePlayer::hasShoted(Point p){
    bool exist = false;
    for(int i=0; i< shotCount; i++) {
        if(shotHistory[i].r == p.r && shotHistory[i].c == p.c) {
            exist = true;
            break;
        }
    }
    return exist;
}

Point MediocrePlayer::randomChoose(){
    int r, c;
    bool exist;
    while(true) {
        exist = false;
        r = randInt(game().rows());
        c = randInt(game().cols());
        
        if(!hasShoted(Point(r,c))) {
            break;
        }
    }
    return Point(r,c);
    
}

Point MediocrePlayer::getNextDirection(){
    Point p;
    bool isThereOpenDirection = false;
    
    if(shotCountInSameDirection == 4) {
//        cout << " shotCountInSameDirection is 4 " << endl;
        for(int i=0; i<4; i++)
        {
            currentDirection = chooseRandomCoordi();
            p = Point(pivotPoint.r + coordinate[currentDirection][0], pivotPoint.c + coordinate[currentDirection][1]);
            if(!isInRange(p)) {
                continue;
            }
            isThereOpenDirection = !hasShoted(p);
            if(isThereOpenDirection){
                break;
            }
        }
        if(!isThereOpenDirection) {
            // all directions are already shoted
            p.r = -1;
            p.c = -1;
        }
    } else {
//        cout << " shotCountInSameDirection is under 4 : " << shotCountInSameDirection << endl;
        shotCountInSameDirection += 1;
        p = Point(lastShotPoint.r + coordinate[currentDirection][0], lastShotPoint.c + coordinate[currentDirection][1]);
        while(true) {
            if(isInRange(p) && !hasShoted(p)) {
                break;
            }
//            cout << "addjust 1" << endl;
            currentDirection = chooseRandomCoordi();
            if(currentDirection == -1) {
                recommendState = 1;
                p = randomChoose();
            } else{
                p = Point(pivotPoint.r + coordinate[currentDirection][0], pivotPoint.c + coordinate[currentDirection][1]);
                shotCountInSameDirection = 1;
            }
            
            
        }
    }
    
    
    return p;
    
}

Point MediocrePlayer::recommendAttack()
{
    if(recommendState == 1) {
//        cout << "state : 1 ";
        if(!lastShotHit) {
//             stay state 1: random choose
            recommendState = 1;
            m_lastCellAttacked = randomChoose();
//            cout << " not hit random ";
        } else {
            if(lastShipDestroyed) {
                // state 1: random choose
                recommendState = 1;
                m_lastCellAttacked = randomChoose();
//                cout << " hit and Destoryed random ";
            } else {
                //
                recommendState = 2;
                resetCoordiHistory();
                currentDirection = chooseRandomCoordi();
                shotCountInSameDirection = 1;
                pivotPoint = lastShotPoint;
                m_lastCellAttacked = getNextDirection();
//                cout << " hit and not Destoryed to next direction stat 2 ";
                if(m_lastCellAttacked.r == -1) {
//                     all directions are already shoted
                    m_lastCellAttacked = randomChoose();
                    recommendState = 1;
//                    cout << " all vivisted randome";
                }
            }
        }
    }
    else if(recommendState == 2) {
//        cout << "state : 2 ";
        if(!lastShotHit) {
            shotCountInSameDirection = 1;
            m_lastCellAttacked = getNextDirection();
//            cout << " not hit and next diection ";
        } else {
            if(lastShipDestroyed) {
                recommendState = 1;
                m_lastCellAttacked = randomChoose();
//                cout << " hit & destroy and random ";
            } else {
                m_lastCellAttacked = getNextDirection();
//                cout << " hit & not destroy and next direction ";
            }
        }
        
    }
    
//    cout << "("<< m_lastCellAttacked.r << "," << m_lastCellAttacked.c << ")" <<endl;
    // Shot history record
    shotHistory[shotCount] = m_lastCellAttacked;
    shotCount += 1;
    return m_lastCellAttacked;
}

void MediocrePlayer::recordAttackResult(Point p, bool validShot,
                                     bool shotHit, bool shipDestroyed,
                                     int shipId)
{
    if(validShot) {
        if(shotHit) {
            lastShotHit = true;
            if(shipDestroyed) {
                lastShipDestroyed = true;
            } else {
                lastShipDestroyed = false;
                lastShotPoint = p;
            }
        } else {
            lastShotHit = false;
            lastShipDestroyed = false;
        }
    }
}

void MediocrePlayer::recordAttackByOpponent(Point p)
{
      // MediocrePlayer completely ignores what the opponent does
}

//*********************************************************************
//  GoodPlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.
//typedef AwfulPlayer GoodPlayer;

class GoodPlayer : public AwfulPlayer
{
public:
    GoodPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
    bool placeShipOnBoard(int numShip, int count, Board &b, const Game &game);
    Point randomChoose();
    Point getNextDirection();
    bool hasShoted(Point p);
    void resetCoordiHistory();
    int chooseRandomCoordi();
    bool isInRange(Point p);
    
    // Good Player Only
    void findNearByPoint();
    bool isInNearPoints(Point p);
    Point getNearByPoint();
    
private:
    Point m_lastCellAttacked;
    
    Point lastShotPoint;
    Point pivotPoint;
    bool lastShotHit;
    bool lastShipDestroyed;
    int recommendState;
    
    Point *shotHistory;
    int shotCount;
    const int coordinate[4][2] = { -1, 0,   // N
                                    0, 1,   // E
                                    1, 0,   // S
                                    0,-1 }; // W
    bool coordiHistory[4];
    int shotCountInSameDirection;
    int currentDirection;
    // GoodPlayer Only
    // When shot count over 50 then try to shot points near by hited points
    Point *hitHistory;
    int hitCount;
    Point *nearByHitPoint;
    int nearCount;
    int useNearCount;

};


GoodPlayer::GoodPlayer(string nm, const Game& g)
 : AwfulPlayer(nm, g), m_lastCellAttacked(0, 0)
{
    lastShotHit= false;
    lastShipDestroyed = false;
    recommendState = 1;
    
    shotHistory = new Point[g.rows()*g.cols()];
    shotCount = 0;
    resetCoordiHistory();
    shotCountInSameDirection = 0;
    
    // GoodPlayer Only
    hitHistory = new Point[g.rows()*g.cols()];
    hitCount = 0;
    nearByHitPoint = new Point[g.rows()*g.cols()];
    nearCount = 0;
    useNearCount = 0;
}

bool GoodPlayer::isInNearPoints(Point p){
    for(int i=0; i< nearCount; i++) {
        if(nearByHitPoint[i].r == p.r && nearByHitPoint[i].c == p.c) {
            return true;
        }
    }
    return false;
}

void GoodPlayer::findNearByPoint(){
//    isInRange
    if(nearCount > 0) {
        // when find is not the first
        delete [] nearByHitPoint;
        nearByHitPoint = new Point[game().rows()* game().cols()];
        nearCount = 0;
        useNearCount = 0;
    }
    
    Point p;
    for(int i=0; i<hitCount; i++) {
        for(int j=0; j<4; j++) {
            p = Point(hitHistory[i].r + coordinate[j][0], hitHistory[i].c + coordinate[j][1]);
            if(!isInNearPoints(p)) {
                if(isInRange(p)) {
                    if(!hasShoted(p)) {
                        nearByHitPoint[nearCount] = p;
                        nearCount++;
                    }
                }
                
            }
        }
    }
//    cout <<"nearCount : " << nearCount << endl;
}

Point GoodPlayer::getNearByPoint(){
    Point p;
    if(useNearCount < nearCount) {
        p = nearByHitPoint[useNearCount];
//        cout <<"getNearByPoint : (" << p.r << ", " << p.c << ")" << endl;
        useNearCount++;
    } else {
        p.r = -1;
        p.c = -1;
    }
    return p;
}

bool GoodPlayer::placeShipOnBoard(int numShip, int count, Board &b, const Game &game) {
    if(numShip == count) {
        return true;
    }
    
    for(int i=0; i < game.rows(); i++) {
        for(int j=0; j<game.cols(); j++) {
            if(b.placeShip(Point(i,j), count, HORIZONTAL)){
                return placeShipOnBoard(numShip, count+1, b, game);
            } else if(b.placeShip(Point(i,j), count, VERTICAL)) {
                return placeShipOnBoard(numShip, count+1, b, game);
            } else {
                b.unplaceShip(Point(i,j), count, HORIZONTAL);
                b.unplaceShip(Point(i,j), count, VERTICAL);
            }
        }
    }
     
    return false;
    
}

bool GoodPlayer::placeShips(Board& b)
{
    for(int i=0; i<50; i++) {
        b.block();
        if(placeShipOnBoard(game().nShips(), 0, b, game())){
            
            break;
        }
        b.unblock();
    }
    b.unblock();
//    b.display(false);
    
    return true;
}

bool GoodPlayer::isInRange(Point p){
    return checkValidPoint(p, game().rows(), game().cols());
}

int GoodPlayer::chooseRandomCoordi(){
    int count =0;
    int coordi = -1;
    
    shotCountInSameDirection = 1;
    
    for(int i=0; i<4; i++) {
        if(coordiHistory[i] == false){
            count++;
            coordi = i;
        }
    }
    
    if(count == 0) {
        return -1; // already visit all direction
    } else if(count == 1) {
        coordiHistory[coordi] = true;
        return coordi;
    } else {
        while(true) {
            coordi = randInt(4);
            if(coordiHistory[coordi] == false) {
                coordiHistory[coordi] = true;
                break;
            }
        }
    }
    return coordi;
}
void GoodPlayer::resetCoordiHistory(){
    for(int i=0; i<4; i++) {
        coordiHistory[i] = false;
    }
}
bool GoodPlayer::hasShoted(Point p){
    bool exist = false;
    for(int i=0; i< shotCount; i++) {
        if(shotHistory[i].r == p.r && shotHistory[i].c == p.c) {
            exist = true;
            break;
        }
    }
    return exist;
}

Point GoodPlayer::randomChoose(){
    int r, c;
    // Good Player Strategy
    // If hit count is over 50, not just random but look first near point by hitted point
    Point p;
    if(shotCount > 20) {
        p = getNearByPoint();
        if(p.r != -1) {
            return p;
        } else {
            // find again
            findNearByPoint();
//            cout << "findNearByPoint " << endl;
        }
    }
    
    bool exist;
    while(true) {
        exist = false;
        r = randInt(game().rows());
        c = randInt(game().cols());
        
        if(!hasShoted(Point(r,c))) {
            break;
        }
    }
    return Point(r,c);
    
}

Point GoodPlayer::getNextDirection(){
    Point p;
    bool isThereOpenDirection = false;
    
    if(shotCountInSameDirection == 4) {
//        cout << " shotCountInSameDirection is 4 " << endl;
        for(int i=0; i<4; i++)
        {
            currentDirection = chooseRandomCoordi();
            p = Point(pivotPoint.r + coordinate[currentDirection][0], pivotPoint.c + coordinate[currentDirection][1]);
            if(!isInRange(p)) {
                continue;
            }
            isThereOpenDirection = !hasShoted(p);
            if(isThereOpenDirection){
                break;
            }
        }
        if(!isThereOpenDirection) {
            // all directions are already shoted
            p.r = -1;
            p.c = -1;
        }
    } else {
//        cout << " good player shotCountInSameDirection is under 4 : " << shotCountInSameDirection << endl;
        shotCountInSameDirection += 1;
        p = Point(lastShotPoint.r + coordinate[currentDirection][0], lastShotPoint.c + coordinate[currentDirection][1]);
        while(true) {
            if(isInRange(p) && !hasShoted(p)) {
                break;
            }
//                cout << "addjust 1" << endl;
            currentDirection = chooseRandomCoordi();
            if(currentDirection == -1) {
                recommendState = 1;
                p = randomChoose();
            } else{
                p = Point(pivotPoint.r + coordinate[currentDirection][0], pivotPoint.c + coordinate[currentDirection][1]);
                shotCountInSameDirection = 1;
            }
        }
    }
    
    
    return p;
    
}

Point GoodPlayer::recommendAttack()
{
    if(recommendState == 1) {
//        cout << "state : 1 ";
        if(!lastShotHit) {
//             stay state 1: random choose
            recommendState = 1;
            m_lastCellAttacked = randomChoose();
//            cout << " not hit random ";
        } else {
            if(lastShipDestroyed) {
                // state 1: random choose
                recommendState = 1;
                m_lastCellAttacked = randomChoose();
//                cout << " hit and Destoryed random ";
            } else {
                //
                recommendState = 2;
                resetCoordiHistory();
                currentDirection = chooseRandomCoordi();
                shotCountInSameDirection = 1;
                pivotPoint = lastShotPoint;
                m_lastCellAttacked = getNextDirection();
//                cout << " hit and not Destoryed to next direction stat 2 ";
                if(m_lastCellAttacked.r == -1) {
//                     all directions are already shoted
                    m_lastCellAttacked = randomChoose();
                    recommendState = 1;
//                    cout << " all vivisted randome";
                }
            }
        }
    }
    else if(recommendState == 2) {
//        cout << "state : 2 ";
        if(!lastShotHit) {
            shotCountInSameDirection = 1;
            m_lastCellAttacked = getNextDirection();
//            cout << " not hit and next diection ";
        } else {
            if(lastShipDestroyed) {
                recommendState = 1;
                m_lastCellAttacked = randomChoose();
//                cout << " hit & destroy and random ";
            } else {
                m_lastCellAttacked = getNextDirection();
//                cout << " hit & not destroy and next direction ";
            }
        }
        
    }
    
//    cout << "("<< m_lastCellAttacked.r << "," << m_lastCellAttacked.c << ")" <<endl;
    // Shot history record
    shotHistory[shotCount] = m_lastCellAttacked;
    shotCount += 1;
    return m_lastCellAttacked;
}

void GoodPlayer::recordAttackResult(Point p, bool validShot,
                                     bool shotHit, bool shipDestroyed,
                                     int shipId)
{
    if(validShot) {
        if(shotHit) {
            // GoodPlayer Only
            hitHistory[hitCount] = p;
            hitCount++;
            // good Player only
            lastShotHit = true;
            if(shipDestroyed) {
                lastShipDestroyed = true;
            } else {
                lastShipDestroyed = false;
                lastShotPoint = p;
            }
        } else {
            lastShotHit = false;
            lastShipDestroyed = false;
        }
    }
    
    // Only Good Player
    if(hitCount == 20) {
        // find Near By Points
        findNearByPoint();
    }
}

void GoodPlayer::recordAttackByOpponent(Point p)
{
      // MediocrePlayer completely ignores what the opponent does
}
//*********************************************************************
//  createPlayer
//*********************************************************************

Player* createPlayer(string type, string nm, const Game& g)
{
    static string types[] = {
        "human", "awful", "mediocre", "good"
    };
    
    int pos;
    for (pos = 0; pos != sizeof(types)/sizeof(types[0])  &&
                                                     type != types[pos]; pos++)
        ;
    switch (pos)
    {
      case 0:  return new HumanPlayer(nm, g);
      case 1:  return new AwfulPlayer(nm, g);
      case 2:  return new MediocrePlayer(nm, g);
      case 3:  return new GoodPlayer(nm, g);
      default: return nullptr;
    }
}
