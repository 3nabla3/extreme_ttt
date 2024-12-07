#include "pch.h"

#include "AppConfig.h"
#include "Board.h"
#include "Game.h"
#include "players/HumanPlayer.h"
#include "players/AIPlayer.h"
#include "players/RandomPlayer.h"

std::unique_ptr<Player> CreatePlayer(std::string playerType, uint8_t AIPlayerDepth) {
  if (playerType == "human") {
    return std::make_unique<HumanPlayer>();
  } else if (playerType == "random") {
    return std::make_unique<RandomPlayer>();
  } else if (playerType == "ai") {
    return std::make_unique<AIPlayer>(AIPlayerDepth);
  }
  SPDLOG_ERROR("Unknown player type: {}", playerType);
  return nullptr;
}

int main(int argc, char* argv[]) {
  spdlog::set_level(spdlog::level::trace);
  SPDLOG_INFO(R"(
     _____      _                             _____ _        _____            _____          
    | ____|_  _| |_ _ __ ___ _ __ ___   ___  |_   _(_) ___  |_   _|_ _  ___  |_   _|__   ___ 
    |  _| \ \/ / __| '__/ _ \ '_ ` _ \ / _ \   | | | |/ __|   | |/ _` |/ __|   | |/ _ \ / _ \
    | |___ >  <| |_| | |  __/ | | | | |  __/   | | | | (__    | | (_| | (__    | | (_) |  __/
    |_____/_/\_\\__|_|  \___|_| |_| |_|\___|   |_| |_|\___|   |_|\__,_|\___|   |_|\___/ \___|
  )");

  std::string initiailBoardStr = R"(
x	x	o	x	o	o	o	o	x
x	x	o	o	x	x	o	o	.
o	o	x	x	x	o	o	.	.
o	x	.	.	o	o	.	x	x
.	o	.	o	x	o	o	x	x
.	x	o	o	x	o	.	x	o
x	x	x	o	.	.	x	x	x
x	.	.	o	x	o	x	x	.
o	x	.	o	o	x	o	.	o
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

  AppConfig config(argc, argv);

  Game game;
  std::unique_ptr<Player> playerX;
  std::unique_ptr<Player> playerO;
  playerX = CreatePlayer(config.GetXPlayerType(), config.GetAiPlayerDepth());
  playerO = CreatePlayer(config.GetOPlayerType(), config.GetAiPlayerDepth());

  game.RegisterPlayer(PlayerSymbol::X, std::move(playerX));
  game.RegisterPlayer(PlayerSymbol::O, std::move(playerO));
  game.Run();

  return 0;
}