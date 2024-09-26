#include "pch.h"
#include "MinMaxPlayer.h"

int MinMaxPlayer::attempted = 0;
int MinMaxPlayer::found = 0;
std::unordered_map<Board, Score> MinMaxPlayer::s_scoreMap;

Move MinMaxPlayer::GetMove() {
  return Move();
}

Score MinMaxPlayer::Negamax(const Board& board, int depth, Score alpha, Score beta, int weight) {
  if (depth == 0 || board.IsGameOver()) {
    return weight * StaticAnalysis(board);
  }
  return alpha + beta;
}

// https://en.wikipedia.org/wiki/Negamax

Score MinMaxPlayer::StaticAnalysis(const Board& board) {
  // find out if Score is cached, and return that instead
  attempted++;
  const auto& result = s_scoreMap.find(board);
  if (result != s_scoreMap.end()) {
    found++;
    return result->second;
  }

  Score score = CalcStaticAnalysis(board);
  s_scoreMap[board] = score;
  return score;
}

Score MinMaxPlayer::CalcStaticAnalysis(const Board&) {
  // TODO
  return Score();
}
