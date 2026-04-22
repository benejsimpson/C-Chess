#include "include/utils.h"
#include "include/fen.hpp"
#include "include/board.hpp"
#include "include/move.hpp"
#include "include/movegen.hpp"

// g++ -std=c++17 -Wall -Wextra -g3 test.cpp board.cpp fen.cpp move.cpp movegen.cpp -o test.exe

const char nl = '\n';

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

    for (int p_ind : (std::vector<int>){48,49,50,51,52,53,54,55})
    {
        generate_pawn_moves(board, moves, p_ind);
        
        std::cout << "\nLEGAL MOVES:\n";
        for (Move m : moves)
        {
            Piece piece_moved = m.piece;
            char p_c = piece_to_char(piece_moved);
            std::cout << "move " << p_c << " from " << m.from << " to " << m.to;
            std::cout << '\n';
        }
    }

    std::cout << "\nPress Enter to exit...";
    std::cin.get();

    return 0;
}