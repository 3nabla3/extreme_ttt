#pragma once
#include "players/Player.h"

class Game {
public:
  Game() = default;
  Game(const Board& board) : m_board(board) {}

  GameStatus Run();
  void RegisterPlayer(std::unique_ptr<Player> player);

private:
  Board m_board;
  std::unique_ptr<Player> m_playerX;
  std::unique_ptr<Player> m_playerO;
};