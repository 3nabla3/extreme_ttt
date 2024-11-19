#pragma once
#include "Player.h"

typedef int32_t Score;

class AIPlayer : public Player {
public:
  AIPlayer() = default;
  virtual void Initialize(PlayerSymbol player, const Board& board) override {
    SPDLOG_TRACE("Initializing MinMaxPlayer with player: {}", player);
    m_player = player;
    m_mainBoard = board;
  }
  virtual void Terminate() override { m_isTerminated = true; }

  virtual Move GetMove() override;
  virtual void ReceiveMove(const Move& move) override;
  virtual void Reset() override {}

  static float HitRatio() { return ((found * 1.0f) / (attempted * 1.0f)); }
  static std::pair<int, int> HitStats() { return {attempted, found}; }

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
  static int attempted;
  static int found;
};