#include "include/utils.h"
#include "include/fen.hpp"
#include "include/board.hpp"

const char nl = '\n';

void PRINT_BOARD(Board board)
{
    for (int rank = 7; rank >= 0; rank--)
    {
        for (int file = 0; file < 8; file++)
        {
            std::cout << piece_to_char(board.squares[file_rank_to_index(file,rank)]) << ' ';
        }
        std::cout << nl;
    }
}

int main()
{
    Board board{};
    load_fen(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

    PRINT_BOARD(board);

    std::cout << "\nPress Enter to exit...";
    std::cin.get();

    return 0;
}