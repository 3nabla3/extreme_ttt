#include "pch.h"
#include "Game.h"
#include "players/MinMaxPlayer.h"

void Game::RegisterPlayer(std::unique_ptr<Player> player) {
  if (!m_playerX) {
    m_playerX = std::move(player);
    m_playerX->SetPlayer(PlayerSymbol::X);
    SPDLOG_INFO("Registed player X");
  } else {
    m_playerO = std::move(player);
    m_playerO->SetPlayer(PlayerSymbol::O);
    SPDLOG_INFO("Registed player O");
  }
}

GameStatus Game::Run() {
  SPDLOG_INFO("Running the game");
  std::cout << m_board << '\n';

  while (!m_board.IsGameOver()) {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1000ms);
    SPDLOG_INFO("Waiting for a move");

    Move m;
    PlayerSymbol player = m_board.GetCurrentPlayer();
    if (player == PlayerSymbol::X) {
      m = m_playerX->GetMove();
      m_board.Play(m);
      m_playerO->ReceiveMove(m);
    } else {
      m = m_playerO->GetMove();
      m_board.Play(m);
      m_playerX->ReceiveMove(m);
    }

    SPDLOG_INFO("{} played {}", player, m);
    SPDLOG_DEBUG("New hash {}", std::hash<Board>{}(m_board));

    std::cout << m_board << '\n';
  }

  SPDLOG_INFO("Game is over");
  return m_board.GetTopGameStatus();
}
