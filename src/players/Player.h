#pragma once
#include "../Board.h"
#include "../Move.h"

class Player {
public:
  virtual bool IsHuman() const { return false; }

  virtual ~Player(){};

  /**
   * Called once when the game starts
   *
   * @param player the player symbol
   * @param initial the initial board
   */
  virtual void Initialize(PlayerSymbol player, const Board& initial) = 0;

  /**
   * Called when the game is terminated
   * either by a game ending or the user
   * closing the game window
   */
  virtual void Terminate() = 0;

  /**
   * Requests the next move,
   * this is the main function of this class
   * if the move is illegal, the game will log an error
   * and ask again at the next iteration
   *
   * @return the next move
   */
  virtual Move GetMove() = 0;

  /**
   * Called when the other player makes a move
   * so this player can update its state
   *
   * @param move the move
   */
  virtual void ReceiveMove(const Move& move) = 0;

  /**
   * Resets the player for the next game
   */
  virtual void Reset() = 0;

  /**
   * Called when the user clicks on the screen
   *
   * @param x the x ratio of the click (0-1)
   * @param y the y ratio of the click (0-1)
   */
  virtual void OnMouseButtonEvent(
      [[maybe_unused]] double x,
      [[maybe_unused]] double y){};
};
