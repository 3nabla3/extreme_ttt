#include "pch.h"
#include <gtest/gtest.h>
#include "players/AIPlayer.h"

using Piece::Empty, Piece::X, Piece::O;

TEST(StaticAnalysis, all_empty) {
  /* clang-format off */
    Piece board[3 * 3] = {
        Empty,  Empty,  Empty,
        Empty,  Empty,  Empty,
        Empty,  Empty,  Empty,
    };
  /* clang-format on */
  ASSERT_EQ(SingleBoardStaticAnalysis(board), 0);
}

TEST(StaticAnalysis, x_middle) {
  /* clang-format off */
    Piece board[3 * 3] = {
        Empty,  Empty,   Empty,
        Empty,  X,       Empty,
        Empty,  Empty,   Empty,
    };
  /* clang-format on */
  ASSERT_EQ(SingleBoardStaticAnalysis(board), 4);
}

TEST(StaticAnalysis, 2_pieces_1) {
  /* clang-format off */
    Piece board[3 * 3] = {
        Empty,  Empty,   O,
        Empty,  X,       Empty,
        Empty,  Empty,   Empty,
    };
  /* clang-format on */
  ASSERT_EQ(SingleBoardStaticAnalysis(board), 3 - 2);
}

TEST(StaticAnalysis, 2_pieces_2) {
  /* clang-format off */
    Piece board[3 * 3] = {
        Empty,  Empty,   Empty,
        Empty,  X,       O,
        Empty,  Empty,   Empty,
    };
  /* clang-format on */
  ASSERT_EQ(SingleBoardStaticAnalysis(board), 3 - 1);
}

TEST(StaticAnalysis, 3_pieces_1) {
  /* clang-format off */
    Piece board[3 * 3] = {
        Empty,  Empty,   Empty,
        Empty,  X,       O,
        Empty,  X,       Empty,
    };
  /* clang-format on */
  ASSERT_EQ(SingleBoardStaticAnalysis(board), 5 - 1);
}

TEST(StaticAnalysis, 3_pieces_2) {
  /* clang-format off */
    Piece board[3 * 3] = {
        Empty,  Empty,   X,
        Empty,  X,       O,
        Empty,  Empty,   Empty,
    };
  /* clang-format on */
  ASSERT_EQ(SingleBoardStaticAnalysis(board), 5 - 0);
}

TEST(StaticAnalysis, example_not_full) {
  /* clang-format off */
    Piece board[3 * 3] = {
        O,      O,   Empty,
        X,      X,   O,
        Empty,  X,   Empty,
    };
  /* clang-format on */
  ASSERT_EQ(SingleBoardStaticAnalysis(board), 2 - 3);
}

TEST(StaticAnalysis, example_full) {
  /* clang-format off */
    Piece board[3 * 3] = {
        O,      O,   X,
        X,      X,   O,
        O,      X,   O,
    };
  /* clang-format on */
  ASSERT_EQ(SingleBoardStaticAnalysis(board), 0);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}