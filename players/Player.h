#pragma once
#include "../Board.h"
#include "../Move.h"

class Player {
public:
  virtual ~Player(){};
  // this is called once when the player gets registered with the game
  virtual void Initialize(PlayerSymbol player, const Board& initial) = 0;

  virtual Move GetMove() = 0;
  virtual void Reset() = 0;
  virtual void ReceiveMove(const Move& move) = 0;
};