#pragma once

#include "Move.h"

enum class Piece {
  X = 1,
  O = -1,
  Empty = 0
};

std::ostream& operator<<(std::ostream& os, const Piece& piece);
template <>
struct fmt::formatter<Piece> : fmt::ostream_formatter {};

enum class PlayerSymbol {
  X = 1,
  O = -1
};

std::ostream& operator<<(std::ostream& os, const PlayerSymbol& player);
template <>
struct fmt::formatter<PlayerSymbol> : fmt::ostream_formatter {};

enum class GameStatus {
  InProgress,
  XWins,
  OWins,
  Draw,
};

std::ostream& operator<<(std::ostream& os, const GameStatus& status);
template <>
struct fmt::formatter<GameStatus> : fmt::ostream_formatter {};

Move ConvertIdxToMove(int idx);

// caches a mapping for a piece in format
// (row * 9 + col) to an index in m_board
extern const std::array<int, 9 * 9> s_boardIndexConversion;

class Board {
public:
  Board() = default;
  Board(std::string boardStr, const Move& lastMove);

  friend bool operator==(const Board& lhs, const Board& rhs);
  bool IsMoveLegal(const Move& move) const;
  std::vector<Move> GetLegalMoves() const;
  void Play(const Move& move);
  PlayerSymbol GetCurrentPlayer() const { return m_currentPlayer; }
  PlayerSymbol GetOtherPlayer() const {
    return static_cast<PlayerSymbol>(-static_cast<int>(m_currentPlayer));
  }
  inline GameStatus GetTopGameStatus() const { return m_topGameStatus; }
  inline std::optional<Move> GetLastMove() const { return m_lastMove; }

  inline bool IsGameOver() const {
    return m_topGameStatus != GameStatus::InProgress;
  }
  friend std::ostream& operator<<(std::ostream& os, const Board& board);
  friend std::size_t hash_value(const Board& board);

  std::array<GameStatus, 9> GetBigBoard() const { return m_bigBoard; }
  inline Piece GetPieceAt(int board, int cell) const { return m_board[board * 9 + cell]; }
  inline Piece GetPieceAtRowCol(int row, int col) const { return m_board[s_boardIndexConversion[row * 9 + col]]; }

private:
  // Get the game status for the entire board
  GameStatus CalcGameStatus(PlayerSymbol currentPlayer) const;
  // Get the game status for a specific sub board
  GameStatus CalcGameStatus(PlayerSymbol currentPlayer, int boardPosition) const;
  GameStatus GetGameStatus_IMPL(PlayerSymbol currentPlayer, const Piece* board) const;

  bool IsBoardFull(int boardPosition) const;
  void PrintPiece(std::ostream& os, int row, int col) const;

  std::array<Piece, 9 * 9> m_board = {Piece::Empty};
  std::array<GameStatus, 9> m_bigBoard = {GameStatus::InProgress};
  GameStatus m_topGameStatus = GameStatus::InProgress;

  PlayerSymbol m_currentPlayer = PlayerSymbol::X;
  std::optional<Move> m_lastMove;

  static const std::array<std::array<int, 3>, 8> s_indices;
};

// make the Board hashable
namespace std {
template <>
struct hash<Board> {
  std::size_t operator()(const Board& board) const {
    return hash_value(board);
  }
};
} // namespace std