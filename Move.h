#pragma once
#include <stdexcept>

struct Move {
  int m_BoardPosition;
  int m_cellPosition;
  Move(int boardPosition, int cellPosition)
      : m_BoardPosition(boardPosition), m_cellPosition(cellPosition) {
    if (boardPosition < 0 || boardPosition > 8 || cellPosition < 0 ||
        cellPosition > 8) {
      throw std::invalid_argument("Invalid move");
    }
  }
  bool operator==(const Move& other) const {
    return m_BoardPosition == other.m_BoardPosition &&
           m_cellPosition == other.m_cellPosition;
  }
};