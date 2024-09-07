#pragma once
#include "PlayerInterface.h"

class RandomPlayer : public PlayerInterface {
public:
  virtual void Reset() override {}
  virtual void SetPlayer(Player player) override { m_player = player; }

  virtual void ReceiveMove(const Move& move) override { m_Board.Play(move); }

  virtual Move GetMove() override {
    while (true) {
      int boardPosition = rand() % 9;
      int cellPosition = rand() % 9;
      Move move(boardPosition, cellPosition);
      if (m_Board.IsMoveLegal(move)) {
        std::cout << m_player << " " << move.m_BoardPosition << ", "
                  << move.m_cellPosition << '\n';

        m_Board.Play(move);
        return move;
      }
    }
  }

private:
  Player m_player;
  Board m_Board;
};