#include "Game.h"

void Game::RegisterPlayer(std::unique_ptr<PlayerInterface> player) {
  if (!m_playerX) {
    m_playerX = std::move(player);
    m_playerX->SetPlayer(Player::X);
  } else {
    m_playerO = std::move(player);
    m_playerX->SetPlayer(Player::O);
  }
}

GameStatus Game::Run() {
  std::cout << m_board << '\n';

  while (!m_board.IsGameOver()) {
    if (m_board.GetCurrentPlayer() == Player::X) {
      Move m = m_playerX->GetMove();
      m_board.Play(m);
      m_playerO->ReceiveMove(m);
    } else {
      Move m = m_playerO->GetMove();
      m_board.Play(m);
      m_playerX->ReceiveMove(m);
    }

    std::cout << m_board << '\n';
  }

  return m_board.GetGameStatus();
}
