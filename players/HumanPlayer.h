#pragma once
#include "Player.h"

class HumanPlayer : public Player {
public:
  virtual void Reset() override {}
  virtual void ReceiveMove(const Move&) override {}
  virtual void Initialize(PlayerSymbol player, const Board&) override {
    m_player = player;
  }

  virtual Move GetMove() override {
    std::cout << "Player " << m_player << " move\n";
    int boardPosition;
    int cellPosition;
    std::cout << "Enter board position: ";
    std::cin >> boardPosition;
    std::cout << "Enter cell position: ";
    std::cin >> cellPosition;
    return Move(boardPosition, cellPosition);
  }

private:
  PlayerSymbol m_player;
};