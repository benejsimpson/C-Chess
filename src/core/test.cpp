#include "core/utils.h"
#include "core/fen.hpp"
#include "core/board.hpp"
#include "core/move.hpp"
#include "core/movegen.hpp"

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

int main()
{
    Board board{};
    std::vector<Move> moves{};
    load_fen(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

    PRINT_BOARD(board);

    std::cout << "\nPress Enter to exit...";
    std::cin.get();

    return 0;
}