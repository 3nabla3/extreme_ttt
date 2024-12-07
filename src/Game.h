#pragma once
#include "players/Player.h"

class Game {
public:
  Game(bool headless = false) : m_headless(headless) { Init(); }
  Game(const Board& board, bool headless = false) : m_board(board), m_headless(headless) { Init(); }
  ~Game();

  // the game takes ownership of the player
  void RegisterPlayer(PlayerSymbol symbol, std::unique_ptr<Player>&& player);
  GameStatus Run();

private:
  void OnKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);
  void Reset();

  void Init() {
    if (m_headless)
      return;

    InitGLFW();
    CreateGLFWWindow();
    InitCallbacks();
  }
  void InitGLFW();
  void CreateGLFWWindow();
  void InitCallbacks();

  void RenderLoop();
  GameStatus GameLoop();

  void SetBackgroundColor();
  float GetColorIntensity(Move move);

  Player& GetCurrentPlayer() {
    return (m_board.GetCurrentPlayer() == PlayerSymbol::X) ? *m_playerX : *m_playerO;
  }

  Player& GetOtherPlayer() {
    return (m_board.GetCurrentPlayer() == PlayerSymbol::X) ? *m_playerO : *m_playerX;
  }

  void RenderSinglePiece(int row, int col);
  void RenderSmallPieces();
  void RenderBigPieces();
  void RenderLegalMoves();

  Board m_board;
  std::unique_ptr<Player> m_playerX;
  std::unique_ptr<Player> m_playerO;

  bool m_headless = false;

  // cant make this a unique_ptr
  // because not a complete type
  GLFWwindow* m_window = nullptr;

  std::mutex m_PauseMutex;
  std::condition_variable m_pauseCondVar;
  std::atomic<bool> m_isPaused = false;
  std::atomic<bool> m_gameShouldClose = false;
  // set whether the game should be reset next game loop
  std::atomic<bool> m_resetFlag = false;

  // Thinking indicator state
  std::atomic<bool> m_isThinking = false;
  float m_spinnerAngle = 0.0f;

  int m_windowWidth = 680;
  int m_windowHeight = 600;
  std::atomic<bool> m_viewportNeedsUpdate{false};

  const std::array<float, 3> m_BackgroundColor = {0.0f, 0.1f, 0.1f};
};