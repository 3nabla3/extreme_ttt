# Extreme / Ultimate Tic Tac Toe

## Introduction

Extreme (or ultimate) Tic Tac Toe is played on a 3x3 grid of standard Tic Tac Toe boards. Your move in one small board determines which board your opponent plays in next. To win, you need to claim three small boards in a row on the larger grid, adding layers of strategy to the classic game.

<!-- TODO: insert picture -->

### Prerequisites

To compile and run this project you need a c++ compiler, and cmake

1. Install a compiler
   `sudo apt install g++` or `sudo apt install clang`

2. Install cmake `sudo apt install cmake`

3. Install dependencies

```
sudo apt install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev mesa-common-dev
```

### Installation

```sh
git clone --recursive https://github.com/3nabla3/extreme_ttt.git
cd extreme_ttt

# make a separate build directory for cmake files
mkdir build
cmake -S . -B build

# compile
cmake --build build --target extreme_ttt -j

# run
./build/extreme_ttt
```
