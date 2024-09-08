#pragma once
#include "players/PlayerInterface.h"

class Game {
public:
  Game() = default;
  GameStatus Run();
  void RegisterPlayer(std::unique_ptr<PlayerInterface> player);

private:
  Board m_board;
  std::unique_ptr<PlayerInterface> m_playerX;
  std::unique_ptr<PlayerInterface> m_playerO;
};