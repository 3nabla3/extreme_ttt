#include <iostream>
#include <memory>

#include "Board.h"
#include "Game.h"
#include "Move.h"
#include "players/HumanPlayer.h"
#include "players/RandomPlayer.h"

int main(void) {
  srand(time(0));

  Game game;
  game.RegisterPlayer(std::make_unique<RandomPlayer>());
  game.RegisterPlayer(std::make_unique<RandomPlayer>());

  GameStatus status = game.Run();
  std::cout << status << '\n';

  return 0;
}
