#include "pch.h"

#include "Board.h"
#include "Game.h"
#include "players/HumanPlayer.h"
#include "players/MinMaxPlayer.h"
#include "players/RandomPlayer.h"

int main(void) {
  spdlog::set_level(spdlog::level::trace);
  SPDLOG_INFO(R"(
     _____      _                             _____ _        _____            _____          
    | ____|_  _| |_ _ __ ___ _ __ ___   ___  |_   _(_) ___  |_   _|_ _  ___  |_   _|__   ___ 
    |  _| \ \/ / __| '__/ _ \ '_ ` _ \ / _ \   | | | |/ __|   | |/ _` |/ __|   | |/ _ \ / _ \
    | |___ >  <| |_| | |  __/ | | | | |  __/   | | | | (__    | | (_| | (__    | | (_) |  __/
    |_____/_/\_\\__|_|  \___|_| |_| |_|\___|   |_| |_|\___|   |_|\__,_|\___|   |_|\___/ \___|
  )");

  std::string initiailBoardStr = R"(
.	.	.	x	x	x	.	o	.
.	o	.	.	o	.	.	o	.
.	o	.	.	x	x	.	.	.
.	.	.	x	o	.	.	o	.
x	.	.	.	x	x	.	o	.
.	.	.	.	x	x	.	o	.
.	.	o	.	.	.	.	o	.
.	.	.	o	.	x	.	o	.
.	x	.	.	x	x	.	o	.
  )";
  Board initialBoard(initiailBoardStr, Move(3, 3));

  //     std::string initiailBoardStr = R"(
  // .	.	.	x	x	x	.	o	.
  // .	o	.	.	o	.	.	o	.
  // .	o	.	.	x	x	.	.	.
  // .	.	.	x	o	.	.	o	.
  // .	.	.	.	x	x	.	o	.
  // x	.	.	.	x	x	.	o	.
  // .	.	x	.	.	o	.	o	.
  // .	.	.	o	.	x	.	o	.
  // .	o	.	.	x	x	.	o	.
  // )";
  //   Board initialBoard(initiailBoardStr, Move(7, 2));

  // Game game(initialBoard);
  Game game;
  game.RegisterPlayer(std::make_unique<MinMaxPlayer>());
  game.RegisterPlayer(std::make_unique<MinMaxPlayer>());

  game.RunGUI();

  SPDLOG_INFO("Cache hit ratio: {}", MinMaxPlayer::HitRatio());
  std::pair<int, int> stats = MinMaxPlayer::HitStats();
  SPDLOG_INFO("\t\tAttempted: {}, Found: {}", stats.first, stats.second);

  return 0;
}