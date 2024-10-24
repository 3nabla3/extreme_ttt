#include "pch.h"
#include "Game.h"
#include "players/MinMaxPlayer.h"
#include "Rendering.h"

void Game::OnKeyPress(GLFWwindow*, int key, int, int action, int) {
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    m_isPaused = !m_isPaused;
    m_pauseCondVar.notify_one();
    SPDLOG_INFO("Game is {}", m_isPaused ? "paused" : "running");
  }
}

void Game::InitGLFW() {
  SPDLOG_TRACE("Initializing GLFW");

  if (!glfwInit()) {
    const char* error;
    glfwGetError(&error);
    SPDLOG_CRITICAL("Failed to initialize GLFW: {}", error);
    abort();
  }

  SPDLOG_TRACE("GLFW initialized");
}

void Game::CreateGLFWWindow() {
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  m_window = glfwCreateWindow(
      m_windowWidth, m_windowHeight, "TicTacToe", nullptr, nullptr);

  if (!m_window) {
    const char* error;
    glfwGetError(&error);
    SPDLOG_CRITICAL("Failed to create GLFW window: {}", error);
    glfwTerminate();
    abort();
  }

  // Associate this window with the game class
  // to use in callbacks
  glfwSetWindowUserPointer(m_window, this);
  glfwSwapInterval(1); // Enable vsync

  SPDLOG_TRACE("GLFW window created");
}

void Game::InitCallbacks() {
  glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
    const char* keyName = glfwGetKeyName(key, scancode);
    // keyname can be null for modifier values
    SPDLOG_DEBUG("KeyPressEvent {} {} {} {} {}", key, scancode, action, mods, keyName ? keyName : "");
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    game->OnKeyPress(window, key, scancode, action, mods);
  });

  glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
    // for now we only want to listen to left clicks
    (void)mods;
    if (action == GLFW_RELEASE)
      return;
    if (button != GLFW_MOUSE_BUTTON_LEFT)
      return;

    double x, y;
    glfwGetCursorPos(window, &x, &y);

    SPDLOG_DEBUG("MouseButtonEvent at ({}, {})", x, y);
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    double rx = x / game->m_windowWidth;
    double ry = y / game->m_windowHeight;

    // only propagate the event to the correct player
    if (game->m_board.GetCurrentPlayer() == PlayerSymbol::X)
      game->m_playerX->OnMouseButtonEvent(rx, ry);
    else
      game->m_playerO->OnMouseButtonEvent(rx, ry);
  });

  glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
    SPDLOG_DEBUG("WindowResizeEvent to {}x{}", width, height);
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));

    game->m_windowWidth = width;
    game->m_windowHeight = height;
    game->m_viewportNeedsUpdate = true;
  });

  SPDLOG_TRACE("Callbacks initialized");
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

  std::thread renderThread(&Game::RenderLoop, this);
  std::thread gameThread(&Game::GameLoop, this);
  while (!glfwWindowShouldClose(m_window)) {
    glfwWaitEvents();
  }
  m_gameShouldClose = true;
  m_playerX->Terminate();
  m_playerO->Terminate();

  m_pauseCondVar.notify_one();
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
    // on resize events
    if (m_viewportNeedsUpdate) {
      glViewport(0, 0, m_windowWidth, m_windowHeight);
      m_viewportNeedsUpdate = false;
    }

    RenderSmallBoards();
    RenderBigBoard();
    RenderSmallPieces();
    RenderBigPieces();

    if (!m_board.IsGameOver())
      RenderLegalMoves();

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
  const int boardSize = 3;
  // we want the big pieces to be thicker than the small
  const float thickness = 7.f;

  // add an offset to the big pieces so the big X does
  // not overlap with the small X in the center
  const float xOffset = -0.02f;
  const float yOffset = -0.07f;
  glLoadIdentity();
  glOrtho(xOffset, boardSize + xOffset, yOffset, boardSize + yOffset, -1, 1);
  for (int row = 0; row < boardSize; ++row) {
    for (int col = 0; col < boardSize; ++col) {
      const GameStatus status = m_board.GetBigBoard()[row * boardSize + col];
      int renderRow = boardSize - 1 - row;
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

void Game::RenderLegalMoves() {
  std::vector<Move> moves = m_board.GetLegalMoves();
  // make a vector of unique boardPositions from the moves vector
  std::vector<int> bps;
  for (const Move& move : moves)
    if (std::find(bps.begin(), bps.end(), move.m_boardPosition) == bps.end())
      bps.push_back(move.m_boardPosition);

  glLoadIdentity();
  int boardSize = 9;
  glOrtho(0, boardSize, 0, boardSize, -1, 1);
  for (int boardPosition : bps)
    RenderBoardBorder(boardPosition);
}

GameStatus Game::GameLoop() {
  SPDLOG_INFO("Running the game");

  while (!m_board.IsGameOver() && !m_gameShouldClose) {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(100ms);

    Player* currentPlayer;
    Player* otherPlayer;
    PlayerSymbol ps = m_board.GetCurrentPlayer();
    if (ps == PlayerSymbol::X) {
      currentPlayer = m_playerX.get();
      otherPlayer = m_playerO.get();
    } else {
      currentPlayer = m_playerO.get();
      otherPlayer = m_playerX.get();
    }

    SPDLOG_INFO("Waiting for a move");
    Move move;
    bool played = false;
    move = currentPlayer->GetMove();
    if (m_board.IsMoveLegal(move)) {
      otherPlayer->ReceiveMove(move);
      played = true;
    } else if (!m_gameShouldClose) {
      SPDLOG_ERROR("Illegal move");
    }

    if (played) {
      SPDLOG_INFO("{} played {}", ps, move);
      m_board.Play(move);
      SPDLOG_DEBUG("New hash {}", std::hash<Board>{}(m_board));
    }

    std::unique_lock<std::mutex> pauseLock(m_PauseMutex);
    m_pauseCondVar.wait(pauseLock, [this] {
      return !m_isPaused || m_gameShouldClose;
    });
  }

  SPDLOG_INFO("Game is over with status {}", m_board.GetTopGameStatus());
  return m_board.GetTopGameStatus();
}
