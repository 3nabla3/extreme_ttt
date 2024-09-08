#include "Board.h"
#include "Game.h"
#include "pch.h"
#include "players/HumanPlayer.h"
#include "players/MinMaxPlayer.h"
#include "players/RandomPlayer.h"

int main(void) {
  srand(time(0));

  Game game;
  game.RegisterPlayer(std::make_unique<MinMaxPlayer>());
  game.RegisterPlayer(std::make_unique<RandomPlayer>());

  GameStatus status = game.Run();
  std::cout << status << '\n';

  return 0;
}
