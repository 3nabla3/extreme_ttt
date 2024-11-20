#pragma once
#include "Player.h"

typedef int32_t Score;

class AIPlayer : public Player {
public:
  AIPlayer() = default;
  AIPlayer(uint8_t depth) : m_depth(depth) {}

  virtual void Initialize(PlayerSymbol player, const Board& board) override {
    SPDLOG_TRACE("Initializing MinMaxPlayer with player: {}", player);
    m_player = player;
    m_mainBoard = board;
  }
  virtual void Terminate() override {
    m_isTerminated = true;
    SPDLOG_INFO("AI stats (player {}):", m_player);
    SPDLOG_INFO("Depth: {}", m_depth);

    SPDLOG_INFO("Cache hit ratio: {}", HitRatio());
    std::pair<int, int> stats = HitStats();
    SPDLOG_INFO("\t\tAttempted: {}, Found: {}", stats.first, stats.second);
  }

  virtual Move GetMove() override;
  virtual void ReceiveMove(const Move& move) override;
  virtual void Reset() override {}

  float HitRatio() { return ((m_found * 1.0f) / (m_attempted * 1.0f)); }
  std::pair<int, int> HitStats() { return {m_attempted, m_found}; }

private:
  Score Negamax(const Board& board, int depth, Score alpha, Score beta, int weigth);

  Score StaticAnalysis(const Board& board);
  Score CalcStaticAnalysis(const Board& board);

  std::vector<std::pair<Move, Board>> GetChildrenBoards(const Board& board);
  PlayerSymbol m_player;
  Board m_mainBoard;
  uint8_t m_depth = 3;
  std::atomic<bool> m_isTerminated = false;

  // static variables for caching and bookkeeping
  static std::unordered_map<Board, Score> s_scoreMap;

  int m_attempted = 0;
  int m_found = 0;
};

Score SingleBoardStaticAnalysis(const Piece* board);