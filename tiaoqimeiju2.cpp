#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <hash_map>
#include <unordered_map>
#include <unordered_set>
#include <ctime>
#define NOMINMAX
#include <windows.h>
#include "board.h"
#include "MyHashTable.h"
using namespace std;
const uint64_t hashSize = 4e9;

void test1();
void test2();
void listAllLegalMoves(Board& board, vector<Loc>& list1, vector<Loc>& list2);
int main()
{
  test2();

  return 0;
}

void test1()
{
  Board::initHash();
  Board board(9, 9);
  board.playMoveAssumeLegal(getLoc(1, 2), getLoc(5, 7));
  board.printBoard(cout);
  uint64_t h = board.minSymHash();
  cout << h << endl;
  board = Board(h);
  board.printBoard(cout);
  board.checkConsistency();
  // vector<Loc> list1,list2;
  // listAllLegalMoves(board, list1, list2);
   //for (int i = 0; i < list1.size(); i++)
   //{
  //   cout << getX(list1[i]) << " " << getY(list1[i]) << "   " << getX(list2[i]) << " " << getY(list2[i])<<endl;
   //}
}

void test2()
{
  double startTime = GetTickCount();
  Board::initHash();

  Board rootboard(9, 9);
  MyHashTable allBoards(hashSize);//有25亿局面
  allBoards.insert_if_not_found(rootboard.minHash(),0);

  int boardNum = 0;



  uint64_t resultHash;
  int solutionStep = 0;




  for(int step=0;step<=13;step++)
  {
    for (auto p = allBoards.data.begin(); p != allBoards.data.end(); p++)
    {
      uint64_t thisBoardHash = *p;
      if (thisBoardHash == 0)continue;
      if ((thisBoardHash>>48)!=step)continue;
      //cout << "1";

      Board thisBoard = Board((thisBoardHash<<16)>>16);
      vector<Loc> list1, list2;
      listAllLegalMoves(thisBoard, list1, list2);
      for (int i = 0; i < list1.size(); i++)
      {
        Board board = thisBoard;
        board.playMoveAssumeLegal(list1[i], list2[i]);
        // board.checkConsistency();
        uint64_t h = board.minHash();
        //cout << "a" << allBoards.find(h) << "b";
        if (allBoards.find(h)!=-1)
        {
          continue;
        }
       // cout << "3";
        if(step<13)allBoards.insert_if_not_found(h,step+1);

        uint64_t hs = board.minSymHash();

        if (allBoards.find(hs)!=-1)
        {
          allBoards.insert_if_not_found(h, step + 1);
          int solStep = int(allBoards.find(hs)+step+1);
          cout << "Found solution, steps=" << solStep << endl;
          if (solutionStep == 0 || solutionStep > solStep)
          {
            solutionStep = solStep;
            resultHash = h;
          }
        }
        if (solutionStep % 2 == 1)break;
      }

      if (solutionStep % 2 == 1)break;


      boardNum++;

      const int infostep = 500000;
      if (boardNum%infostep == 0)
      {
        cout << "step=" << step
          << "  processed=" << boardNum
          << "  total=" << allBoards.used
          << "  current = " << double(p-allBoards.data.begin())/hashSize
          << "  time = " << (GetTickCount() - startTime) / 1000
          << endl;
      }


   
    }
    cout << "Finished Step = " << step << ", total board num=" << boardNum << endl << endl;

    if (solutionStep !=0)break;
    //if (boardNum >= 3000000)break;
  }

  // print the path
  uint64_t h = resultHash;
  vector<Board> back, forward;
  back.push_back(h);

  Board backBoard = Board(h);
  for (int backStep = allBoards.find(h)  - 1; backStep >= 0; backStep--)
  {
    vector<Loc> list1, list2;
    listAllLegalMoves(backBoard, list1, list2);
    for (int i = 0; i < list1.size(); i++)
    {
      Board board = backBoard;
      board.playMoveAssumeLegal(list1[i], list2[i]);
      uint64_t h1 = board.minHash();
      if (allBoards.find(h1)== backStep)
      {
        backBoard = board;
        back.push_back(board);
        break;
      }
    }

  }

  Board forwardBoard = Board(h);
  for (int forwardStep = allBoards.find(forwardBoard.minSymHash()) - 1; forwardStep >= 0; forwardStep--)
  {
    vector<Loc> list1, list2;
    listAllLegalMoves(forwardBoard, list1, list2);
    for (int i = 0; i < list1.size(); i++)
    {
      Board board = forwardBoard;
      board.playMoveAssumeLegal(list1[i], list2[i]);
      uint64_t h1 = board.minSymHash();
      if (allBoards.find(h1) == forwardStep)
      {
        forwardBoard = board;
        forward.push_back(board);
        break;
      }
    }

  }

  for (int i = forward.size() - 1; i >= 0; i--)
  {
    cout << "step " << forward.size() - 1 - i << endl << forward[i] << endl;
  }
  for (int i = 0; i < back.size(); i++)
  {
    cout << "step " << forward.size() + i << endl << back[i] << endl;
  }

  system("pause");
  system("pause");
  system("pause");

  return;
}










void listAllLegalMoves(Board& board, vector<Loc>& list1, vector<Loc>& list2)
{
  list1.clear();
  list2.clear();
  bool legalMap[Board::MAX_ARR_SIZE];
  for (int x1 = 0; x1 < 9; x1++)
    for (int y1 = 0; y1 < 9; y1++)
    {
      Loc loc1 = getLoc(x1, y1);
      if (board.colors[loc1])//发现棋子
      {
        board.setLegalMap(loc1, legalMap);
        for (int x2 = 0; x2 < 9; x2++)
          for (int y2 = 0; y2 < 9; y2++)
          {
            Loc loc2 = getLoc(x2, y2);
            if (legalMap[loc2])
            {
              list1.push_back(loc1);
              list2.push_back(loc2);
            }

          }


      }
    }
}
