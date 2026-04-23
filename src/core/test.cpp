#include "core/utils.h"
#include "core/fen.hpp"
#include "core/board.hpp"
#include "core/move.hpp"
#include "core/movegen.hpp"
#include <stdexcept>
#include <string>

void PRINT_BOARD(Board board)
{
    for (int rank = 7; rank >= 0; rank--)
    {
        for (int file = 0; file < 8; file++)
        {
            Piece p = board.squares[file_rank_to_index(file,rank)];
            std::cout << piece_to_char(p) << ' ';
        }
        std::cout << nl;
    }
}

void expect_equal(const std::string& actual, const std::string& expected, const char* label)
{
    if (actual != expected)
    {
        throw std::runtime_error(
            std::string(label) + "\nExpected: " + expected + "\nActual:   " + actual);
    }
}

int main()
{
    Board board{};
    load_fen(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq");
    expect_equal(
        export_fen(board),
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq",
        "Start position should preserve all castling rights");

    apply_move(board, Move{12, 28, WP, Empty, DOUBLE_PAWN, Empty}); // e2 -> e4
    apply_move(board, Move{52, 36, BP, Empty, DOUBLE_PAWN, Empty}); // e7 -> e5
    expect_equal(
        export_fen(board),
        "rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq",
        "Ordinary opening pawn moves should not remove castling rights");

    PRINT_BOARD(board);
    std::cout << "\nAll castling-rights checks passed.\n";

    return 0;
}
