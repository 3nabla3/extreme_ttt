#pragma once
#include <iostream>
#include <optional>

#include "Move.h"

enum class Piece {
  X = 1,
  O = -1,
  Empty = 0
};

std::ostream& operator<<(std::ostream& os, const Piece& piece);

enum class Player {
  X = 1,
  O = -1
};

std::ostream& operator<<(std::ostream& os, const Player& piece);

enum class GameStatus {
  InProgress,
  XWins,
  OWins,
  Draw,
};

std::ostream& operator<<(std::ostream& os, const GameStatus& status);

class Board {
public:
  Board() = default;
  bool IsMoveLegal(const Move& move) const;
  void Play(const Move& move);
  Player GetCurrentPlayer() const { return m_currentPlayer; }
  Player GetOtherPlayer() const {
    return static_cast<Player>(-static_cast<int>(m_currentPlayer));
  }

  // Get the game status for the entire board
  GameStatus GetGameStatus() const;
  // Get the game status for a specific sub board
  GameStatus GetGameStatus(int boardPosition) const;

  inline bool IsGameOver() const {
    return GetGameStatus() != GameStatus::InProgress;
  }
  friend std::ostream& operator<<(std::ostream& os, const Board& board);

private:
  bool IsBoardFull(int boardPosition) const;
  void PrintPiece(std::ostream& os, int idx) const;
  GameStatus GetGameStatus_IMPL(const Piece* board) const;

  Piece m_board[9 * 9] = {Piece::Empty};
  GameStatus m_bigBoard[9] = {GameStatus::InProgress};

  Player m_currentPlayer = Player::X;
  std::optional<Move> m_lastMove;

  static int s_indices[8][3];
};