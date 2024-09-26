#include "pch.h"

#include "Board.h"
#include "Game.h"
#include "players/HumanPlayer.h"
#include "players/MinMaxPlayer.h"
#include "players/RandomPlayer.h"

int main(void) {
  spdlog::set_level(spdlog::level::info);
  SPDLOG_INFO(R"(
     _____      _                             _____ _        _____            _____          
    | ____|_  _| |_ _ __ ___ _ __ ___   ___  |_   _(_) ___  |_   _|_ _  ___  |_   _|__   ___ 
    |  _| \ \/ / __| '__/ _ \ '_ ` _ \ / _ \   | | | |/ __|   | |/ _` |/ __|   | |/ _ \ / _ \
    | |___ >  <| |_| | |  __/ | | | | |  __/   | | | | (__    | | (_| | (__    | | (_) |  __/
    |_____/_/\_\\__|_|  \___|_| |_| |_|\___|   |_| |_|\___|   |_|\__,_|\___|   |_|\___/ \___|
  )");

  Game game;
  game.RegisterPlayer(std::make_unique<RandomPlayer>());
  game.RegisterPlayer(std::make_unique<RandomPlayer>());

  GameStatus status = game.Run();
  SPDLOG_INFO("Game exited with status {}", status);
  return 0;
}
