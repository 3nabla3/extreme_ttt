#pragma once
#include "PlayerInterface.h"

class HumanPlayer : public PlayerInterface {
public:
  virtual void Reset() override {}
  virtual void ReceiveMove(const Move& move) override {}
  virtual void SetPlayer(Player player) override { m_player = player; }

  virtual Move GetMove() override {
    std::cout << "Player " << m_player << " move" << std::endl;
    int boardPosition;
    int cellPosition;
    std::cout << "Enter board position: ";
    std::cin >> boardPosition;
    std::cout << "Enter cell position: ";
    std::cin >> cellPosition;
    return Move(boardPosition, cellPosition);
  }

private:
  Player m_player;
};