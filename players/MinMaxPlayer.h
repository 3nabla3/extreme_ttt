#pragma once
#include "Player.h"

typedef uint32_t Score;

class MinMaxPlayer : public Player {
public:
  virtual void Reset() override {}
  virtual void ReceiveMove(const Move&) override {}
  virtual void SetPlayer(PlayerSymbol player) override { m_player = player; }
  virtual Move GetMove() override;
  Score Negamax(const Board& board, int depth, Score alpha, Score beta, int weigth);
  Score StaticAnalysis(const Board& board);
  static float HitRatio() { return ((found * 1.0f) / (attempted * 1.0f)); }

private:
  Score CalcStaticAnalysis(const Board& board);
  static std::unordered_map<Board, Score> s_scoreMap;
  static int attempted;
  static int found;
  PlayerSymbol m_player;
  Board m_mainBoard;
};