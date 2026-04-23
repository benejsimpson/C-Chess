#include "core/utils.h"
#include "core/fen.hpp"
#include "core/board.hpp"

const std::string START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

void load_fen(Board &board, const std::string &fen)
{
    clear_board(board);

    std::string placement = fen;
    std::string active_colour = "w";

    const std::size_t first_space = fen.find(' ');
    if (first_space != std::string::npos)
    {
        placement = fen.substr(0, first_space);

        const std::size_t second_space = fen.find(' ', first_space + 1);
        active_colour = fen.substr(
            first_space + 1,
            second_space == std::string::npos ? std::string::npos : second_space - first_space - 1
        );
    }

    int rank = 7;
    int file = 0;

    for (char c : placement)
    {
        if (c == '/')
        {
            rank--;
            file = 0;
            continue;
        }

        if ('0' < c && c <= '8')
        {
            file += (c - '0');
            continue;
        }

        if (rank < 0 || file > 7)
            break;

        const Piece piece = char_to_piece(c);
        if (piece != Empty)
        {
            place_piece(board, rank * 8 + file, piece);
        }

        file++;
    }

    board.white_to_move = (active_colour != "b");
}

std::string export_fen(const Board& board)
{
    std::string fen;

    for (int rank = 7; rank >= 0; rank--)
    {
        int empty_count = 0;

        for (int file = 0; file < 8; file++)
        {
            const Piece piece = board.squares[file_rank_to_index(file, rank)];
            if (piece == Empty)
            {
                ++empty_count;
                continue;
            }

            if (empty_count > 0)
            {
                fen += static_cast<char>('0' + empty_count);
                empty_count = 0;
            }

            fen += piece_to_char(piece);
        }

        if (empty_count > 0)
        {
            fen += static_cast<char>('0' + empty_count);
        }

        if (rank > 0)
        {
            fen += '/';
        }
    }

    fen += board.white_to_move ? " w" : " b";
    return fen;
}
