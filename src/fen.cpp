#include "include/utils.h"
#include "include/fen.hpp"
#include "include/board.hpp"

const std::string START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

// implement load_fen
void load_fen(Board &board, const std::string &fen)
{
    Piece p{};
    int rank = 7;
    int file = 0;

    for (char c : fen)
    {
        if (c == '/')
        {
            rank --;
            file = 0;
            continue;
        }

        if ('0' < c && c <= '8') // digit in fen
        {
            std::cout << "skipping " << c << " squares\n";
            file += (c - '0');
            continue;
        }
        // pieces
        int index = rank * 8 + file;

        board.squares[index] = char_to_piece(c);
        std::cout << "placing a " << c << " at " << file << rank << '\n';

        file++;
    }
}

// implement export_fen

