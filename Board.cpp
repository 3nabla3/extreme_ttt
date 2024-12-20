#include "pch.h"
#include "Board.h"

const std::array<std::array<int, 3>, 8> Board::s_indices = {{
    // rows
    {{0, 1, 2}},
    {{3, 4, 5}},
    {{6, 7, 8}},
    // columns
    {{0, 3, 6}},
    {{1, 4, 7}},
    {{2, 5, 8}},
    // diags
    {{0, 4, 8}},
    {{2, 4, 6}},
}};

/**
 * Initializes the mapping for s_boardIndexConversion
 */
constexpr std::array<int, 9 * 9> calcBoardIndexConversion() {
  std::array<int, 9 * 9> cache;

  int strIdx = 0;
  int boardIdx = 0;
  for (int bigRow = 0; bigRow < 3; bigRow++) {
    for (int bigCol = 0; bigCol < 3; bigCol++) {
      for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
          cache[boardIdx++] = strIdx++;
        }
        boardIdx += 6; // Skip to the next row in the big grid
      }
      boardIdx -= 24; // Move back to the start of the next big cell
    }
    boardIdx += 18; // Skip to the next big row
  }

  return cache;
}

const std::array<int, 9 * 9> s_boardIndexConversion = calcBoardIndexConversion();

std::ostream& operator<<(std::ostream& os, const Piece& piece) {
  switch (piece) {
  case Piece::X:
    os << "X";
    break;
  case Piece::O:
    os << "O";
    break;
  case Piece::Empty:
    os << ".";
    break;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const PlayerSymbol& player) {
  switch (player) {
  case PlayerSymbol::X:
    os << "X";
    break;
  case PlayerSymbol::O:
    os << "O";
    break;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const GameStatus& status) {
  switch (status) {
  case GameStatus::XWins:
    os << "X wins";
    break;
  case GameStatus::OWins:
    os << "O wins";
    break;
  case GameStatus::Draw:
    os << "Draw";
    break;
  case GameStatus::InProgress:
    os << "In progress";
    break;
  }
  return os;
}

static Piece player2piece(PlayerSymbol player) {
  if (player == PlayerSymbol::X) {
    return Piece::X;
  } else {
    return Piece::O;
  }
}

Board::Board(std::string boardStr, const Move& lastMove)
    : m_lastMove(lastMove) {
  // remove all whitespace
  boardStr.erase(std::remove_if(boardStr.begin(), boardStr.end(), ::isspace),
                 boardStr.end());
  // count number of X and O
  uint64_t xCount = std::count(boardStr.begin(), boardStr.end(), 'x');
  uint64_t oCount = std::count(boardStr.begin(), boardStr.end(), 'o');

  // check if the board is valid
  if (xCount < oCount || xCount > oCount + 1 || boardStr.size() != 9 * 9) {
    SPDLOG_CRITICAL("Invalid board string");
  }

  for (int i = 0; i < 9 * 9; i++) {
    char c = boardStr[i];
    Piece piece;
    if (c == 'x') {
      piece = Piece::X;
    } else if (c == 'o') {
      piece = Piece::O;
    } else {
      piece = Piece::Empty;
    }
    int col = i % 9;
    int row = i / 9;
    int idx = s_boardIndexConversion[row * 9 + col];
    m_board[idx] = piece;
  }

  // Now we need to update the status of the big board
  // by going through each of the small boards and
  // checking if the game is in progress or if one
  // of the players has won
  for (int i = 0; i < 9; i++) {
    m_bigBoard[i] = CalcGameStatus(PlayerSymbol::X, i);
    if (m_bigBoard[i] == GameStatus::XWins)
      continue;

    m_bigBoard[i] = CalcGameStatus(PlayerSymbol::O, i);
  }

  // and then we need to update the status of the
  // entire game
  m_topGameStatus = CalcGameStatus(PlayerSymbol::X);
  if (m_topGameStatus != GameStatus::XWins)
    m_topGameStatus = CalcGameStatus(PlayerSymbol::O);

  // now we can set the actual current player
  m_currentPlayer = xCount == oCount ? PlayerSymbol::X : PlayerSymbol::O;

  SPDLOG_DEBUG("Initialized board with last move {}", lastMove);
}

bool Board::IsBoardFull(int boardPosition) const {
  const auto& start = m_board.begin() + boardPosition * 9;
  const auto& end = start + 9;
  return std::all_of(start, end, [](Piece piece) {
    return piece != Piece::Empty;
  });
}

bool Board::IsMoveLegal(const Move& move) const {
  // no matter what, you can only play in a cell that is empty
  if (m_board[move.m_boardPosition * 9 + move.m_cellPosition] != Piece::Empty) {
    return false;
  }
  // if it's the first move, you can play anywhere
  if (!m_lastMove) {
    return true;
  }
  // you can only play on a board where the game is in progress
  if (m_bigBoard[move.m_boardPosition] != GameStatus::InProgress) {
    return false;
  }
  // if it's not the first move, you can only play in the board
  // that corresponds to the cell of the last move unless that board has
  // a game that that is not in progress
  if (move.m_boardPosition != m_lastMove->m_cellPosition) {
    GameStatus status = m_bigBoard[m_lastMove->m_cellPosition];
    return status != GameStatus::InProgress;
  }
  return true;
}

std::vector<Move> Board::GetLegalMoves() const {
  std::vector<Move> moves;
  for (int i = 0; i < 9 * 9; i++) {
    Move move = ConvertIdxToMove(i);
    if (IsMoveLegal(move))
      moves.push_back(move);
  }
  return moves;
}

void Board::Play(const Move& move) {
  if (!IsMoveLegal(move)) {
    SPDLOG_CRITICAL("Invalid move {}", move);
  }
  int index = move.m_boardPosition * 9 + move.m_cellPosition;
  m_board[index] = player2piece(m_currentPlayer);
  m_lastMove = move;

  // update the status of the big board
  m_bigBoard[move.m_boardPosition] = CalcGameStatus(m_currentPlayer, move.m_boardPosition);
  m_topGameStatus = CalcGameStatus(m_currentPlayer);

  // switch current player
  m_currentPlayer = GetOtherPlayer();
}

Move ConvertIdxToMove(int idx) { return Move(idx / 9, idx % 9); }

void Board::PrintPiece(std::ostream& os, int row, int col) const {
  int idx = s_boardIndexConversion[row * 9 + col];
  const Piece piece = m_board[idx];
  const Move move = ConvertIdxToMove(idx);
  if (m_lastMove && move == *m_lastMove) {
    // red
    os << "\033[1;31m" << piece << "\033[0m";
  } else if (m_bigBoard[move.m_boardPosition] == GameStatus::XWins) {
    // purple
    os << "\033[1;35m" << piece << "\033[0m";
  } else if (m_bigBoard[move.m_boardPosition] == GameStatus::OWins) {
    // light blue
    os << "\033[1;36m" << piece << "\033[0m";
  } else {
    os << piece;
  }
  os << " | ";
}

std::ostream& operator<<(std::ostream& os, const Board& game) {
  os << "|--- --- ---|--- --- ---|--- --- ---|\n| ";
  for (int row = 0; row < 9; row++) {
    for (int col = 0; col < 9; col++) {
      game.PrintPiece(os, row, col);
    }
    if (row % 3 == 2) {
      os << "\n|--- --- ---|--- --- ---|--- --- ---| ";
    }
    if (row != 8) {
      os << "\n| ";
    }
  }
  return os;
}

GameStatus Board::GetGameStatus_IMPL(PlayerSymbol currentPlayer, const Piece* board) const {
  // only the last player that played in the board can win
  Piece piece = player2piece(currentPlayer);
  for (const auto [idx0, idx1, idx2] : s_indices) {
    if (board[idx0] == piece &&
        board[idx1] == piece &&
        board[idx2] == piece) {
      return currentPlayer == PlayerSymbol::X ? GameStatus::XWins
                                              : GameStatus::OWins;
    }
  }

  // check if the board is full
  if (std::all_of(board, board + 9, [](Piece piece) {
        return piece != Piece::Empty;
      })) {
    return GameStatus::Draw;
  }

  return GameStatus::InProgress;
}

GameStatus Board::CalcGameStatus(PlayerSymbol currentPlayer, int boardPosition) const {
  return GetGameStatus_IMPL(currentPlayer, m_board.data() + boardPosition * 9);
}

GameStatus Board::CalcGameStatus(PlayerSymbol player) const {
  // only the last player that played on the board can win
  GameStatus status = player == PlayerSymbol::X ? GameStatus::XWins
                                                : GameStatus::OWins;
  for (const auto& [idx0, idx1, idx2] : s_indices) {
    if (m_bigBoard[idx0] == status &&
        m_bigBoard[idx1] == status &&
        m_bigBoard[idx2] == status) {
      return status;
    }
  }

  // if no boards are in progress, the game is a draw
  if (std::all_of(m_bigBoard.begin(), m_bigBoard.end(), [](GameStatus status) {
        return status != GameStatus::InProgress;
      })) {
    return GameStatus::Draw;
  }

  return GameStatus::InProgress;
}

std::size_t hash_value(const Board& board) {
  std::size_t seed = 0;
  std::hash<int> int_hasher;

  // Hash the main board
  for (const auto& piece : board.m_board) {
    seed ^= int_hasher(static_cast<int>(piece)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  // Hash the big board status
  for (const auto& status : board.m_bigBoard) {
    seed ^= int_hasher(static_cast<int>(status)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  // Hash the current player
  seed ^= int_hasher(static_cast<int>(board.m_currentPlayer)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

  // Hash the last move, if it exists
  if (board.m_lastMove) {
    seed ^= int_hasher(board.m_lastMove->m_cellPosition) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  return seed;
}

bool operator==(const Board& lhs, const Board& rhs) {
  return lhs.m_board == rhs.m_board &&
         lhs.m_bigBoard == rhs.m_bigBoard &&
         lhs.m_currentPlayer == rhs.m_currentPlayer &&
         lhs.m_lastMove == rhs.m_lastMove;
}