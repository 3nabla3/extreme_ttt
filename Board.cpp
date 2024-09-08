#include "Board.h"
#include "pch.h"

int Board::s_indices[8][3] = {
    // rows
    {0, 1, 2},
    {3, 4, 5},
    {6, 7, 8},
    // columns
    {0, 3, 6},
    {1, 4, 7},
    {2, 5, 8},
    // diags
    {0, 4, 8},
    {2, 4, 6},
};

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

std::ostream& operator<<(std::ostream& os, const Player& player) {
  switch (player) {
  case Player::X:
    os << "X";
    break;
  case Player::O:
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

static Piece player2piece(Player player) {
  if (player == Player::X) {
    return Piece::X;
  } else {
    return Piece::O;
  }
}

bool Board::IsBoardFull(int boardPosition) const {
  const Piece* start = m_board + boardPosition * 9;
  return std::all_of(start, start + 9,
                     [](Piece piece) { return piece != Piece::Empty; });
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

void Board::Play(const Move& move) {
  if (!IsMoveLegal(move)) {
    std::stringstream ss;
    ss << "Invalid move: " << move;
    throw std::invalid_argument(ss.str());
  }
  m_board[move.m_boardPosition * 9 + move.m_cellPosition] =
      player2piece(m_currentPlayer);
  m_currentPlayer = static_cast<Player>(-static_cast<int>(m_currentPlayer));
  m_lastMove = move;

  // update the status of the big board
  m_bigBoard[move.m_boardPosition] = GetGameStatus(move.m_boardPosition);
}

static Move ConvertIdxToMove(int idx) { return Move(idx / 9, idx % 9); }

void Board::PrintPiece(std::ostream& os, int idx) const {
  const Piece piece = m_board[idx];
  const Move move = ConvertIdxToMove(idx);
  if (m_lastMove && move == *m_lastMove) {
    os << "\033[1;31m" << piece << "\033[0m";
  } else {
    os << piece;
  }
  os << " | ";
}

std::ostream& operator<<(std::ostream& os, const Board& game) {
  for (int j = 0; j < 3; j++) {
    os << "|--- --- ---|--- --- ---|--- --- ---|\n| ";
    for (int i = 0; i < 9 * 3; i++) {
      if (i % 9 < 3) {
        game.PrintPiece(os, 9 * 3 * j + i);
      }
    }
    os << "\n| ";
    for (int i = 0; i < 9 * 3; i++) {
      if (i % 9 >= 3 && i % 9 < 6) {
        game.PrintPiece(os, 9 * 3 * j + i);
      }
    }
    os << "\n| ";
    for (int i = 0; i < 9 * 3; i++) {
      if (i % 9 >= 6) {
        game.PrintPiece(os, 9 * 3 * j + i);
      }
    }
    os << "\n";
  }
  os << "|--- --- ---|--- --- ---|--- --- ---|\n";
  return os;
}

GameStatus Board::GetGameStatus_IMPL(const Piece* board) const {
  // check if the board is full
  if (std::all_of(board, board + 9,
                  [](Piece piece) { return piece != Piece::Empty; })) {
    return GameStatus::Draw;
  }

  // only the last player that played in the board can win
  Piece piece = player2piece(GetOtherPlayer());
  for (int i = 0; i < 8; i++) {
    const int* indices = s_indices[i];
    if (board[indices[0]] == piece && board[indices[1]] == piece &&
        board[indices[2]] == piece) {
      return GetOtherPlayer() == Player::X ? GameStatus::XWins
                                           : GameStatus::OWins;
    }
  }

  return GameStatus::InProgress;
}

GameStatus Board::GetGameStatus(int boardPosition) const {
  return GetGameStatus_IMPL(m_board + boardPosition * 9);
}

GameStatus Board::GetGameStatus() const {
  // if no boards are in progress, the game is a draw
  if (std::all_of(m_bigBoard, m_bigBoard + 9, [](GameStatus status) {
        return status != GameStatus::InProgress;
      })) {
    return GameStatus::Draw;
  }

  // only the last player that played on the board can win
  GameStatus status =
      GetOtherPlayer() == Player::X ? GameStatus::XWins : GameStatus::OWins;
  for (int i = 0; i < 8; i++) {
    const int* indices = s_indices[i];
    if (m_bigBoard[indices[0]] == status && m_bigBoard[indices[1]] == status &&
        m_bigBoard[indices[2]] == status) {
      return status;
    }
  }

  return GameStatus::InProgress;
}
