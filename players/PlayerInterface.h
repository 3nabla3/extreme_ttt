#pragma once
#include "../Board.h"
#include "../Move.h"

class PlayerInterface {
public:
  virtual ~PlayerInterface(){};
  virtual Move GetMove() = 0;
  virtual void Reset() = 0;
  virtual void ReceiveMove(const Move& move) = 0;
  virtual void SetPlayer(Player player) = 0;
};