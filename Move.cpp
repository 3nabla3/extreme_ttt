#include "pch.h"
#include "Move.h"

std::ostream& operator<<(std::ostream& os, const Move& move) {
  os << "Move(" << move.m_boardPosition << ", " << move.m_cellPosition << ")";
  return os;
}