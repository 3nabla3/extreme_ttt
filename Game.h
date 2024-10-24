#pragma once
#include "players/Player.h"

class Game {
public:
  Game() = default;
  Game(const Board& board) : m_board(board) {}
  ~Game();

  void RegisterPlayer(std::unique_ptr<Player> player);

  GameStatus RunGUI();
  void InitGLFW();
  void RenderLoop();
  GameStatus GameLoop();

  void OnKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
  void SetBackgroundColor();
  float GetColorIntensity(Move move);

  void RenderSinglePiece(int row, int col);
  void RenderSmallPieces();
  void RenderBigPieces();
  void RenderLegalMoves();

  Board m_board;
  std::unique_ptr<Player> m_playerX;
  std::unique_ptr<Player> m_playerO;

  // cant make this a unique_ptr
  // because not a complete type
  GLFWwindow* m_window = nullptr;

  std::mutex m_PauseMutex;
  std::condition_variable m_pauseCondVar;
  std::atomic<bool> m_isPaused = false;

  std::atomic<bool> m_gameShouldClose = false;

  int m_windowWidth = 640;
  int m_windowHeight = 480;
  std::atomic<bool> m_viewportNeedsUpdate{false};

  const std::array<float, 3> m_BackgroundColor = {0.0f, 0.1f, 0.1f};
};