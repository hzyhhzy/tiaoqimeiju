/*
 * board.h
 * Originally from an unreleased project back in 2010, modified since.
 * Authors: brettharrison (original), David Wu (original and later modifications).
 */


#ifndef GAME_BOARD_H_
#define GAME_BOARD_H_
#include <algorithm>
typedef unsigned char Player;
static const Player P_BLACK = 1;
static const Player P_WHITE = 2;

//Color of a point on the board
typedef unsigned char Color;
static const Color C_EMPTY = 0;
static const Color C_BLACK = 1;
static const Color C_WHITE = 2;
static const Color C_WALL = 3;


struct Board;


typedef short Loc;
Loc getLoc(int x, int y, int x_size=9);
int getX(Loc loc, int x_size=9);
int getY(Loc loc, int x_size=9);

void getAdjacentOffsets(short adj_offsets[8], int x_size);
bool isAdjacent(Loc loc0, Loc loc1, int x_size);


struct Board
{
	//CForbiddenPointFinder forbiddenFinder;
  //Initialization------------------------------
  //Initialize the zobrist hash.
  //MUST BE CALLED AT PROGRAM START!
  static void initHash();
  static uint64_t combnum[11][82];//combnum[a][b]=C(a,b)

  //Board parameters and Constants----------------------------------------
   
  static const int MAX_LEN =9;  //Maximum edge length allowed for the board
  static const int MAX_PLAY_SIZE = MAX_LEN * MAX_LEN;  //Maximum number of playable spaces
  static const int MAX_ARR_SIZE = (MAX_LEN+1)*(MAX_LEN+2)+1; //Maximum size of arrays needed



  //Constructors---------------------------------
  Board();  //Create Board of size (19,19)
  Board(int x, int y); //Create Board of size (x,y)
  Board(uint64_t hash); //Create Board of size (x,y)
  Board(const Board& other);

  Board& operator=(const Board&) = default;

  //Functions------------------------------------

public:

	void setLegalMap(Loc chosenLoc, bool* legalMap);
	void setLegalMapIter(Loc startLoc, bool isFirst, bool* legalMap);

  //Check if this location is on the board
  bool isOnBoard(Loc loc) const;

  Color getColor(Loc loc) const;
  
  void playMoveAssumeLegal(Loc loc1,Loc loc2);
  void checkConsistency() const;
  uint64_t boardHash(int sym);
  uint64_t minHash()
  {
    return std::min(boardHash(0), boardHash(1));
  }
  uint64_t minSymHash()
  {
    return std::min(boardHash(2), boardHash(3));
  }
  static const int x_size=9;                  //Horizontal size of board
  static const int y_size=9;                  //Vertical size of board
  Color colors[MAX_ARR_SIZE];  //Color of each location on the board.

  //bool legalMap[MAX_ARR_SIZE];  //Color of each location on the board.



  static short adj_offsets[8]; //Indices 0-3: Offsets to add for adjacent points. Indices 4-7: Offsets for diagonal points.

  void init(int xS, int yS);
  void printBoard(std::ostream& out);
  void printSymBoard(std::ostream& out);
  friend std::ostream& operator<<(std::ostream& out, Board& board);


};




#endif // GAME_BOARD_H_
