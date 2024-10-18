#pragma once
#include "Player.h"

class RandomPlayer : public Player {
public:
  RandomPlayer() {
    std::random_device dev;
    m_rng = std::mt19937(dev());
  }
  virtual void Reset() override {}
  virtual void Initialize(PlayerSymbol player, const Board& board) override {
    m_player = player;
    m_board = board;
  }

  virtual void ReceiveMove(const Move& move) override { m_board.Play(move); }

  virtual Move GetMove() override {
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, 8);
    while (true) {
      int boardPosition = dist(m_rng);
      int cellPosition = dist(m_rng);
      Move move(boardPosition, cellPosition);
      if (m_board.IsMoveLegal(move)) {
        m_board.Play(move);
        return move;
      }
    }
  }

private:
  PlayerSymbol m_player;
  Board m_board;
  std::mt19937 m_rng;
};