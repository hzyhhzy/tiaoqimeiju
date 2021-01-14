#include "board.h"

/*
 * board.cpp
 * Originally from an unreleased project back in 2010, modified since.
 * Authors: brettharrison (original), David Wu (original and later modificationss).
 */

#include <algorithm>
#include <cassert>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

short Board::adj_offsets[8];
uint64_t Board::combnum[11][82];

Loc getLoc(int x, int y, int x_size)
{
  return (x+1) + (y+1)*(x_size+1);
}
int getX(Loc loc, int x_size)
{
  return (loc % (x_size+1)) - 1;
}
int getY(Loc loc, int x_size)
{
  return (loc / (x_size+1)) - 1;
}
void getAdjacentOffsets(short adj_offsets[8], int x_size)
{
	//  604          6
	//  2 3   ->    2 0
	//  517        5 x 4
	//              1 3
	//               7
  adj_offsets[0] = -(x_size+1);   
  adj_offsets[2] = -1;
  adj_offsets[3] = 1;
  adj_offsets[1] = (x_size+1);
  adj_offsets[6] = -(x_size+1)-1;
  adj_offsets[4] = -(x_size+1)+1;
  adj_offsets[7] = (x_size+1)+1;
  adj_offsets[5] = (x_size+1)-1;
//0到5是相连
}

bool isAdjacent(Loc loc0, Loc loc1, int x_size)
{
  return loc0 == loc1 - (x_size+1) || loc0 == loc1 - 1 || loc0 == loc1 + 1 || loc0 == loc1 + (x_size+1);
}

#define FOREACHADJ(BLOCK) {int ADJOFFSET = -(x_size+1); {BLOCK}; ADJOFFSET = -1; {BLOCK}; ADJOFFSET = 1; {BLOCK}; ADJOFFSET = x_size+1; {BLOCK}};
#define ADJ0 (-(x_size+1))
#define ADJ1 (-1)
#define ADJ2 (1)
#define ADJ3 (x_size+1)

//CONSTRUCTORS AND INITIALIZATION----------------------------------------------------------

Board::Board()
{
  init(Board::MAX_LEN, Board::MAX_LEN);
}

Board::Board(int x, int y)
{
  init(x,y);
}
Board::Board(uint64_t hash)
{
  std::fill(colors, colors + MAX_ARR_SIZE, C_WALL);
  for (int y = 0; y < 9; y++)
  {
    for (int x = 0; x < 9; x++)
    {
      Loc loc = (x + 1) + (y + 1)*(9 + 1);
      colors[loc] = C_EMPTY;
    }
  }
  int laststone = 81;
  for (int stone = 10; stone >= 1; stone--)
  {
    for (int i = laststone-1; i >= 0; i--)
    {
      if (hash >= combnum[stone][i])
      {
        hash -= combnum[stone][i];
        laststone = i ;
        Loc loc = i + i / 9 + 11;
        colors[loc] = 1;
        break;
      }
    }
  }

}

Board::Board(const Board& other)
{

  memcpy(colors, other.colors, sizeof(Color)*MAX_ARR_SIZE);

}

void Board::init(int xS=9, int yS=9)
{

  for (int i = 0; i < MAX_ARR_SIZE; i++)
  {
	  colors[i] = C_WALL;
  }

  
  for (int y = 0; y < y_size; y++)
  {
	  for (int x = 0; x < x_size; x++)
	  {
		  Loc loc = (x + 1) + (y + 1)*(x_size + 1);
		  colors[loc] = C_EMPTY;
	  }
  }
  for (int x = 0; x<4; x++)
	  for (int y = 0; y < 4 - x; y++)
	  {
		  Loc loc = getLoc(x, y);
		  colors[loc] = C_BLACK;
	  }

}

uint64_t Board::boardHash(int sym)
{
  int stone = 10;
  uint64_t hash=0;
  if (sym == 0)
  {
    for (int pos = 80; pos >= 0; pos--)
    {
      int x = pos % 9;
      int y = pos / 9;
      //if (sym % 2) { int tmp = x; x = y; y = tmp; }
      //if (sym / 2) { x = 8 - x; y = 8 - y; }
      Loc loc = (x + 1) + (y + 1)*(x_size + 1);
      if (colors[loc])
      {
        hash += combnum[stone][pos];
        stone--;
      }
    }
  }
  else if (sym == 1)//除了int x=....y=...以外与上面完全相同
  {
    for (int pos = 80; pos >= 0; pos--)
    {
      int y = pos % 9;
      int x = pos / 9;
      //if (sym % 2) { int tmp = x; x = y; y = tmp; }
      //if (sym / 2) { x = 8 - x; y = 8 - y; }
      Loc loc = (x + 1) + (y + 1)*(x_size + 1);
      if (colors[loc])
      {
        hash += combnum[stone][pos];
        stone--;
      }
    }
  }
  else if (sym == 2)//除了int x=....y=...以外与上面完全相同
  {
    for (int pos = 80; pos >= 0; pos--)
    {
      int x = 8 - pos % 9;
      int y = 8 - pos / 9;
      //if (sym % 2) { int tmp = x; x = y; y = tmp; }
      //if (sym / 2) { x = 8 - x; y = 8 - y; }
      Loc loc = (x + 1) + (y + 1)*(x_size + 1);
      if (colors[loc])
      {
        hash += combnum[stone][pos];
        stone--;
      }
    }
  }
  else if (sym == 3)//除了int x=....y=...以外与上面完全相同
  {
    for (int pos = 80; pos >= 0; pos--)
    {
      int y = 8 - pos % 9;
      int x = 8 - pos / 9;
      //if (sym % 2) { int tmp = x; x = y; y = tmp; }
      //if (sym / 2) { x = 8 - x; y = 8 - y; }
      Loc loc = (x + 1) + (y + 1)*(x_size + 1);
      if (colors[loc])
      {
        hash += combnum[stone][pos];
        stone--;
      }
    }
  }
  return hash;
}


void Board::initHash()
{
  getAdjacentOffsets(adj_offsets, 9);

  for (int i = 0; i < 82; i++)
  {
    combnum[0][i] = 1;
  }
  for (int i = 1; i <= 10; i++)
  {
    for (int j = 0; j < i; j++)combnum[i][j] = 0;
    for (int j = i; j < 82; j++)combnum[i][j] = combnum[i][j-1]+ combnum[i-1][j - 1];
  }
  //for (int i = 0; i <= 10; i++)
  //for (int j = 0; j <= 81; j++)
  //cout << i << " " << j << " " << combnum[i][j] << endl;
}

bool Board::isOnBoard(Loc loc) const {
  return loc >= 0 && loc < MAX_ARR_SIZE && colors[loc] != C_WALL;
}
Color Board::getColor(Loc loc)const
{
	if (!isOnBoard(loc))return C_WALL;
	else return colors[loc];
}
void Board::setLegalMap(Loc chosenLoc, bool* legalMap)
{
  std::fill(legalMap, legalMap + MAX_ARR_SIZE, false);
	legalMap[chosenLoc] = true;
	setLegalMapIter(chosenLoc, true, legalMap);
	for (int i = 0; i < 6; i++)
	{
		Loc loc = chosenLoc + adj_offsets[i];
		if (colors[loc] == C_EMPTY)legalMap[loc] = true;
	}
  legalMap[chosenLoc] = false;
	

}
void Board::setLegalMapIter(Loc startLoc,bool isFirst, bool* legalMap)
{

	if (!isOnBoard(startLoc))return;
	if (isFirst)
	{
		if (colors[startLoc] !=1)
		{
			cout << "colors[startLoc] != chosenColor";
			return;
		}
	}
	else if (colors[startLoc] != C_EMPTY)
	{
		return;
	}
	if (!isFirst&&legalMap[startLoc])return;
	legalMap[startLoc] = 1;
	for (int i = 0; i < 6; i++)
	{
		Loc loc = startLoc + adj_offsets[i];
		if (colors[loc] == 1)setLegalMapIter(startLoc + 2 * adj_offsets[i], false, legalMap);
	}
	
}

void Board::playMoveAssumeLegal(Loc loc1,Loc loc2)
{
  if (colors[loc1] != 1 || colors[loc2] != 0)cout << "color error\n";
  colors[loc1] = 0;
  colors[loc2] = 1;
}


void Board::checkConsistency() const {
  string errLabel = string("Board::checkConsistency(): ");

  int tmpBlackStoneNum = 0;
  for(Loc loc = 0; loc < MAX_ARR_SIZE; loc++) {
    int x = getX(loc,x_size);
    int y = getY(loc,x_size);
    if(x < 0 || x >= x_size || y < 0 || y >= y_size) {
      if(colors[loc] != C_WALL)
        cout << (errLabel + "Non-WALL value outside of board legal area");
    }
    else {
      if(colors[loc] == C_BLACK ) {
		    tmpBlackStoneNum++;
      }
      else if(colors[loc] == C_EMPTY) {
      }
      else
        cout << (errLabel + "Non-(black,white,empty) value within board legal area");
    }
  }
  if (tmpBlackStoneNum != 10)cout << "Not 10 stones";

  short tmpAdjOffsets[8];
  getAdjacentOffsets(tmpAdjOffsets,x_size);
  for(int i = 0; i<8; i++)
    if(tmpAdjOffsets[i] != adj_offsets[i])
      cout<<(errLabel + "Corrupted adj_offsets array");
}


void Board::printBoard(ostream& out) {
  for(int y1 = 0; y1 <y_size; y1++)
  {
	  for (int i = 0; i < y_size - y1; i++)out << " ";
    for(int x1 = 0; x1 <= y1; x1++)
    {
		  int y = y1 - x1;
		  int x = x1;
      Loc loc = getLoc(x,y);
      char s =colors[loc]?'O':'.';
      out << s;
      out << " ";

    }
    out << "\n";
  }
  for (int y1 = y_size; y1 < y_size*2-1; y1++)
  {
	  int xlen = y_size * 2 - y1-1;
	  for (int i = 0; i <  y1-y_size+2; i++)out << " ";
	  for (int x1 = 0; x1 < xlen; x1++)
	  {
		  int y = y_size-1-x1;
		  int x = y1 - y_size+x1+1;
		  Loc loc = getLoc(x, y, x_size);
		  char s = colors[loc] ? 'O' : '.';
			out << s;
      out << " ";

	  }
	  out << "\n";
  }
  out << "\n";
}
ostream& operator<<(ostream& out, Board& board) {
  board.printBoard(out);
  return out;
}