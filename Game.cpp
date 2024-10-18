#include "pch.h"
#include "Game.h"
#include "players/MinMaxPlayer.h"
#include "Rendering.h"

static void KeyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods) {
  Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
  game->KeyCallback(window, key, scancode, action, mods);
}

void Game::KeyCallback(GLFWwindow*, int key, int, int action, int) {
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    isPaused = !isPaused;
    pauseCondVar.notify_one();
    SPDLOG_INFO("Game is {}", isPaused ? "paused" : "running");
  }
}

void Game::InitGLFW() {
  SPDLOG_TRACE("Initializing GLFW");

  if (!glfwInit()) {
    SPDLOG_CRITICAL("Failed to initialize GLFW");
    throw std::runtime_error("Failed to initialize GLFW");
  }

  // TODO: allow resizing later on
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  m_window = glfwCreateWindow(
      m_windowWidth, m_windowHeight, "TicTacToe", nullptr, nullptr);

  if (!m_window) {
    glfwTerminate();
    SPDLOG_CRITICAL("Failed to create GLFW window");
    throw std::runtime_error("Failed to create GLFW window");
  }

  glfwSetWindowUserPointer(m_window, this);
  glfwSetKeyCallback(m_window, KeyCallbackWrapper);

  glfwSwapInterval(1); // Enable vsync
  SPDLOG_TRACE("GLFW initialized");
}

Game::~Game() {
  if (m_window) {
    SPDLOG_TRACE("Destroying GLFW window");
    glfwDestroyWindow(m_window);
  }
  SPDLOG_TRACE("Terminating GLFW");
  glfwTerminate();
}

void Game::RegisterPlayer(std::unique_ptr<Player> player) {
  if (!m_playerX) {
    m_playerX = std::move(player);
    m_playerX->Initialize(PlayerSymbol::X, m_board);
    SPDLOG_INFO("Registed player X");
  } else {
    m_playerO = std::move(player);
    m_playerO->Initialize(PlayerSymbol::O, m_board);
    SPDLOG_INFO("Registed player O");
  }
}

GameStatus Game::RunGUI() {
  SPDLOG_TRACE("Running GUI");
  if (!m_window)
    InitGLFW();

  std::thread renderThread(&Game::RenderLoop, this);
  std::thread gameThread(&Game::GameLoop, this);
  while (!glfwWindowShouldClose(m_window)) {
    glfwWaitEvents();
  }
  gameShouldClose = true;
  pauseCondVar.notify_one();

  renderThread.join();
  SPDLOG_TRACE("Render thread joined");
  gameThread.join();
  SPDLOG_TRACE("Game thread joined");

  return m_board.GetTopGameStatus();
}

/**
 * @brief Converts RGB values to a hexadecimal color string
 * @param r Red component of the color (0.0-1.0)
 * @param g Green component of the color (0.0-1.0)
 * @param b Blue component of the color (0.0-1.0)
 * @return A hexadecimal color string in the format #RRGGBB
 */
std::string convertToHex(float r, float g, float b) {
  int red = static_cast<int>(r * 255);
  int green = static_cast<int>(g * 255);
  int blue = static_cast<int>(b * 255);

  std::stringstream ss;
  ss << "#"
     << std::setw(2) << std::setfill('0') << std::hex << red
     << std::setw(2) << std::setfill('0') << std::hex << green
     << std::setw(2) << std::setfill('0') << std::hex << blue;

  return ss.str();
}

void Game::SetBackgroundColor() {
  float r = m_BackgroundColor[0];
  float g = m_BackgroundColor[1];
  float b = m_BackgroundColor[2];

  std::string hex = convertToHex(r, g, b);
  SPDLOG_DEBUG("Setting background color to {}", hex);
  glClearColor(r, g, b, 1.0f);
}

void Game::RenderLoop() {
  // make sure the context is current on this thread
  glfwMakeContextCurrent(m_window);

  SetBackgroundColor();
  while (!glfwWindowShouldClose(m_window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    RenderSmallBoards();
    RenderBigBoard();
    RenderSmallPieces();
    RenderBigPieces();

    glfwSwapBuffers(m_window);
  }

  glfwMakeContextCurrent(nullptr);
}

float Game::GetColorIntensity(Move move) {
  GameStatus bigBoardStatus = m_board.GetBigBoard()[move.m_boardPosition];

  if (move == m_board.GetLastMove())
    return 1.f;
  else if (bigBoardStatus != GameStatus::InProgress)
    return 0.3f;
  else
    return 0.7f;
}

void Game::RenderSinglePiece(int row, int col) {
  const Piece piece = m_board.GetPieceAtRowCol(row, col);
  // the board is rendered upside down
  int renderRow = 8 - row;
  int idx = s_boardIndexConversion[row * 9 + col];
  Move m = ConvertIdxToMove(idx);

  if (piece == Piece::X) {
    GLfloat red[3] = {GetColorIntensity(m), 0.f, 0.f};
    RenderX(renderRow, col, red);
  } else if (piece == Piece::O) {
    GLfloat blue[3] = {0.f, 0.f, GetColorIntensity(m)};
    RenderO(renderRow, col, blue);
  }
}

void Game::RenderSmallPieces() {
  const int boardSize = 9;
  glLoadIdentity();
  glOrtho(0, boardSize, 0, boardSize, -1, 1);

  for (int row = 0; row < boardSize; ++row) {
    for (int col = 0; col < boardSize; ++col) {
      RenderSinglePiece(row, col);
    }
  }
}

void Game::RenderBigPieces() {
  const int kBoardSize = 3;
  // we want the big pieces to be thicker than the small
  const float thickness = 7.f;

  glLoadIdentity();
  glOrtho(0, kBoardSize, 0, kBoardSize, -1, 1);
  for (int row = 0; row < kBoardSize; ++row) {
    for (int col = 0; col < kBoardSize; ++col) {
      const GameStatus status = m_board.GetBigBoard()[row * kBoardSize + col];
      int renderRow = kBoardSize - 1 - row;
      if (status == GameStatus::XWins) {
        GLfloat red[3] = {0.6f, 0.f, 0.f};
        RenderX(renderRow, col, red, thickness);
      } else if (status == GameStatus::OWins) {
        GLfloat blue[3] = {0.f, 0.f, 0.6f};
        RenderO(renderRow, col, blue, thickness);
      }
    }
  }
}

GameStatus Game::GameLoop() {
  SPDLOG_INFO("Running the game");

  while (!m_board.IsGameOver() && !gameShouldClose) {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(100ms);

    Move move;
    PlayerSymbol player = m_board.GetCurrentPlayer();
    SPDLOG_INFO("Waiting for a move");
    if (player == PlayerSymbol::X) {
      move = m_playerX->GetMove();
      m_playerO->ReceiveMove(move);
    } else {
      move = m_playerO->GetMove();
      m_playerX->ReceiveMove(move);
    }

    SPDLOG_INFO("{} played {}", player, move);
    m_board.Play(move);
    SPDLOG_DEBUG("New hash {}", std::hash<Board>{}(m_board));

    // std::cout << m_board << '\n';

    std::unique_lock<std::mutex> pauseLock(pauseMutex);
    pauseCondVar.wait(pauseLock, [this] { return !isPaused || gameShouldClose; });
  }

  SPDLOG_INFO("Game is over with status {}", m_board.GetTopGameStatus());
  return m_board.GetTopGameStatus();
}
