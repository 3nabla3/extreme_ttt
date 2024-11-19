#include "pch.h"
#include "AIPlayer.h"

int AIPlayer::attempted = 0;
int AIPlayer::found = 0;
std::unordered_map<Board, Score> AIPlayer::s_scoreMap;

Move AIPlayer::GetMove() {
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

void AIPlayer::ReceiveMove(const Move& move) {
  m_mainBoard.Play(move);
}

std::vector<std::pair<Move, Board>> AIPlayer::GetChildrenBoards(const Board& board) {
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

Score AIPlayer::Negamax(const Board& board, int depth, Score alpha, Score beta, int weight) {
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

/**
 * @brief Calculates the score of the board from the perspective of player X
 *
 * The higher the score, the better it is for player X. This function should be
 * called with a board that is not in a terminal state.
 *
 * @param board The board to evaluate
 * @return The score of the board
 *
 * This function uses a simple memoization technique to avoid recalculating
 * the same board multiple times. It keeps a map of the boards that have been
 * evaluated, and returns the stored score if the same board is encountered
 * again.
 */
Score AIPlayer::StaticAnalysis(const Board& board) {
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

/**
 * @brief Calculates the score of the board from the perspective of player X
 *
 * The higher the score, the better it is for player X. This function should be
 * called with a board that is not in a terminal state.
 *
 * @param board The board to evaluate
 * @return The score of the board
 */
Score AIPlayer::CalcStaticAnalysis(const Board& board) {
  if (board.GetTopGameStatus() == GameStatus::XWins) {
    return 100;
  } else if (board.GetTopGameStatus() == GameStatus::OWins) {
    return -100;
  } else if (board.GetTopGameStatus() == GameStatus::Draw) {
    return 0;
  }

  return 0;
}

/**
 * @brief Calculates the score of a single small board
 * from the perspective of player X
 *
 * The higher the score, the better it is for player X. This function should be
 * called with a board that is not in a terminal state.
 *
 * @param board The board to evaluate
 * @return The score of the board
 */
Score SingleBoardStaticAnalysis(const Piece* ) {
  // for (auto [idx0, idx1, idx2] : Board::s_indices) {
  // }
  return 0;
}

// You're right that the current implementation of CalcStaticAnalysis is quite basic
// for Ultimate Tic Tac Toe. Here are some ideas to improve the static analysis
// heuristic:

// Evaluate sub-board wins: Count how many sub-boards each player has won.
// Weighted sub-board evaluation: Assign different weights to sub-boards based
// on their strategic importance (e.g., center and corner boards might be more
// valuable).
// Potential winning lines: Count how many potential winning lines each player
// has on the main board. A potential winning line is one where the player has
// won some sub-boards and the rest are still in play.
// Sub-board state evaluation: For each sub-board that isn't won yet, evaluate
// its state (similar to regular Tic Tac Toe evaluation).
// Control of the next move: If the next move sends the opponent to a
// favorable board, reduce the score slightly.
// Blocking opportunities: Reward positions that block the opponent from
// completing a winning line on the main board.
// Center control: Give extra points for controlling the center sub-board, as
// it's often strategically important.
// Corner control: Similar to center control, but with less weight.
// Threat assessment: Evaluate immediate threats (where the opponent could win
// a sub-board or the main board on their next move).
// Mobility: Consider how many legal moves are available. More options are
// generally better.
// The final score could be a weighted sum of these factors, normalized to fit
// within your desired range (e.g., -100 to 100). You'd need to experiment with
// the weights to find a balance that produces good play.
