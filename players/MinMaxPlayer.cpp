#include "pch.h"
#include "MinMaxPlayer.h"

int MinMaxPlayer::attempted = 0;
int MinMaxPlayer::found = 0;
std::unordered_map<Board, Score> MinMaxPlayer::s_scoreMap;

Move MinMaxPlayer::GetMove() {
  std::vector<std::pair<Move, Board>> boards = GetChildrenBoards(m_mainBoard);
  Score bestValue = std::numeric_limits<Score>::min();
  Move bestMove;

  Score alpha = std::numeric_limits<Score>::min();
  Score beta = std::numeric_limits<Score>::max();

  PlayerSymbol player = m_mainBoard.GetCurrentPlayer();
  SPDLOG_DEBUG("Player is {}", player);
  int weight = -static_cast<int>(player);
  SPDLOG_DEBUG("Weight is {}", weight);

  SPDLOG_DEBUG("Analyzing {} possible moves (higher is better)", boards.size());
  for (const auto& [move, board] : boards) {
    if (m_isTerminated)
      break;

    SPDLOG_DEBUG("Analyzing move {}", move);
    Score value = -Negamax(board, m_depth, alpha, beta, weight);
    SPDLOG_DEBUG("\t --> score {} (best value: {})", value, bestValue);
    if (value > bestValue) {
      SPDLOG_DEBUG("New best move {}", move);
      bestValue = value;
      bestMove = move;
    }
  }

  // apply the move to our main board
  m_mainBoard.Play(bestMove);
  return bestMove;
}

void MinMaxPlayer::ReceiveMove(const Move& move) {
  m_mainBoard.Play(move);
}

std::vector<std::pair<Move, Board>> MinMaxPlayer::GetChildrenBoards(const Board& board) {
  // TODO: can this be optimized
  std::vector<std::pair<Move, Board>> boards;

  for (int boardPosition = 0; boardPosition < 9; boardPosition++) {
    for (int cellPosition = 0; cellPosition < 9; cellPosition++) {
      Move move(boardPosition, cellPosition);
      if (board.IsMoveLegal(move)) {
        Board board_copy = board;
        board_copy.Play(move);
        boards.push_back({move, board_copy});
      }
    }
  }

  return boards;
}

Score MinMaxPlayer::Negamax(const Board& board, int depth, Score alpha, Score beta, int weight) {
  // this function returns the score from the perspective
  // of the player who's turn it is to play.
  // The higher the score, the better it is for the player

  if (depth == 0 || board.IsGameOver()) {
    Score sa = StaticAnalysis(board);
    return weight * sa;
  }

  std::vector<std::pair<Move, Board>> boards = GetChildrenBoards(board);
  // order moves (might do this later)
  // std::sort(boards.begin(), boards.end(), [&](const Board& a, const Board& b) {
  //   return StaticAnalysis(a) > StaticAnalysis(b);
  // });

  Score bestValue = std::numeric_limits<Score>::min();
  for (const auto& [_move, child] : boards) {
    Score value = -Negamax(child, depth - 1, -beta, -alpha, -weight);
    bestValue = std::max(bestValue, value);
    alpha = std::max(alpha, value);
    if (alpha >= beta) {
      break;
    }
  }
  return bestValue;
}

// https://en.wikipedia.org/wiki/Negamax

Score MinMaxPlayer::StaticAnalysis(const Board& board) {
  // A simple memoization technique to avoid recalculating the same board
  attempted++;
  const auto& it = s_scoreMap.find(board);
  if (it != s_scoreMap.end()) {
    found++;
    return it->second;
  }

  Score score = CalcStaticAnalysis(board);
  s_scoreMap[board] = score;
  return score;
}

Score MinMaxPlayer::CalcStaticAnalysis(const Board& board) {
  // TODO: this function should return the score of the board
  // from the perspective of player X. Meaning that the higher
  // the score, the better it is for player X

  if (board.GetTopGameStatus() == GameStatus::XWins) {
    return 100;
  } else if (board.GetTopGameStatus() == GameStatus::OWins) {
    return -100;
  } else if (board.GetTopGameStatus() == GameStatus::Draw) {
    return 0;
  }

  return 0;
}
