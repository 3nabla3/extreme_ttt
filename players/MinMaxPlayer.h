#pragma once
#include "PlayerInterface.h"

struct Node {
  Move move;
  Board board;
  std::vector<std::unique_ptr<Node>> children;
};

class MinMaxPlayer : public PlayerInterface {
public:
  virtual void Reset() override {}
  virtual void ReceiveMove(const Move& move) override {}
  virtual void SetPlayer(Player player) override { m_player = player; }

  virtual Move GetMove() override { return Move(0, 0); }

private:
  Player m_player;
};