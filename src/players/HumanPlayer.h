#pragma once
#include "Player.h"

class HumanPlayer : public Player {
public:
  virtual void Initialize(PlayerSymbol player, const Board&) override {
    m_player = player;
  }

  virtual void Terminate() override {
    m_isTerminated = true;
    m_cv.notify_all();
  }

  virtual Move GetMove() override {
    // wait until a move is chosen
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cv.wait(lock, [this] {
      return m_chosenMove.has_value() || m_isTerminated;
    });

    if (!m_chosenMove) {
      // this can occur if we want to close the
      // game while the player is thinking
      // since we have to return something to get
      // back to the game loop we arbitrarily
      // return 0,0 and the game loop will check
      // if it is illegal
      return Move(0, 0);
    }

    return std::exchange(m_chosenMove, std::nullopt).value();
  }

  virtual void ReceiveMove(const Move&) override {}
  virtual void Reset() override {}

  virtual void OnMouseButtonEvent(double x, double y) override {
    int row = static_cast<int>(y * 9);
    int col = static_cast<int>(x * 9);
    int idx = s_boardIndexConversion[row * 9 + col];
    Move move = ConvertIdxToMove(idx);
    std::unique_lock<std::mutex> lock(m_mutex);
    m_chosenMove = move;
    m_cv.notify_all();
  }

private:
  PlayerSymbol m_player;
  std::optional<Move> m_chosenMove = std::nullopt;

  std::mutex m_mutex;
  std::condition_variable m_cv;
  std::atomic<bool> m_isTerminated = false;
};