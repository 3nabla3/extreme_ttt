#pragma once
#include "../Board.h"
#include "../Move.h"

class Player {
public:
  virtual ~Player(){};
  virtual Move GetMove() = 0;
  virtual void Reset() = 0;
  virtual void ReceiveMove(const Move& move) = 0;
  virtual void SetPlayer(PlayerSymbol player) = 0;
};