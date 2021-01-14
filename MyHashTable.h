#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;
class MyHashTable
{
public:
  uint64_t size;
  uint64_t used;
  vector<uint64_t> data;
  MyHashTable()
  {
    MyHashTable(128);
  }
  MyHashTable(uint64_t size) :size(size), used(0), data(size + 1000, 0)
  {
  }

  uint64_t cal_hash(uint64_t x)
  {
    x ^= x >> 33;
    x *= 0xff51afd7ed558ccdULL;
    x ^= x >> 33;
    x *= 0xc4ceb9fe1a85ec53ULL;
    x ^= x >> 33;
    return x;
  }

  int insert_if_not_found(uint64_t board, uint64_t step)//找到了返回步数，没找到返回-1
  {
    auto hash = cal_hash(board);
    hash = hash % size;
    board = board | (step << 48);
    for (uint64_t i = hash;; i++)
    {
      uint64_t d = data[i];
      if (d == 0)
      {
        used++;
        if (used > size*0.8)cout << "Hash Table Full\n";
        data[i] = board;
        return -1;
      }
      else if ((d << 16) == (board << 16))
      {
        return d >> 48;
      }
    }
  }

  int find(uint64_t board)//找到了返回步数，没找到返回-1
  {
    auto hash = cal_hash(board);
    hash = hash % size;
    for (uint64_t i = hash;; i++)
    {
      auto d = data[i];
      if ( d== 0)
      {
        return -1;
      }
      else if ((d << 16) == (board << 16))
      {
        return d >> 48;
      }
    }


  }

};

