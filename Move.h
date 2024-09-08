#pragma once

struct Move {
  int m_boardPosition;
  int m_cellPosition;
  Move(int boardPosition, int cellPosition)
      : m_boardPosition(boardPosition), m_cellPosition(cellPosition) {
    if (boardPosition < 0 || boardPosition > 8 || cellPosition < 0 ||
        cellPosition > 8) {
      throw std::invalid_argument("Invalid move");
    }
  }
  bool operator==(const Move& other) const {
    return m_boardPosition == other.m_boardPosition &&
           m_cellPosition == other.m_cellPosition;
  }
};

std::ostream& operator<<(std::ostream& os, const Move& move);