#include "core/utils.h"
#include "core/fen.hpp"
#include "core/board.hpp"

const std::string START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq";

void load_fen(Board &board, const std::string &fen)
{
    clear_board(board);

    std::string placement = "";
    std::string active_colour = "w";
    std::string castling = "-";
    std::string en_passant = "-";

    // split FEN into parts
    // 1 : piece placement
    // 2 : side to move
    // 3 : castling rights
    // 4 : en passant
    std::size_t first_space = fen.find(' ');
    std::size_t second_space = std::string::npos;
    std::size_t third_space = std::string::npos;
    std::size_t fourth_space = std::string::npos;

    if (first_space == std::string::npos)
    {
        placement = fen;
    }
    else
    {
        placement = fen.substr(0, first_space);

        second_space = fen.find(' ', first_space + 1);

        active_colour = fen.substr(
            first_space + 1,
            second_space == std::string::npos
                ? std::string::npos
                : second_space - first_space - 1);

        if (second_space != std::string::npos)
        {
            std::size_t third_space = fen.find(' ', second_space + 1);

            castling = fen.substr(
                second_space + 1,
                third_space == std::string::npos
                    ? std::string::npos
                    : third_space - second_space - 1);
        }

        if (third_space != std::string::npos)
        {
            fourth_space = fen.find(' ', third_space + 1);

            en_passant = fen.substr(
                third_space + 1,
                fourth_space == std::string::npos
                    ? std::string::npos
                    : fourth_space - third_space - 1);
        }
    }

    // load board pieces
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

    // side to move
    board.white_to_move = (active_colour != "b");

    // reset castling rights then enable whats in FEN
    board.white_king_side = false;
    board.white_queen_side = false;
    board.black_king_side = false;
    board.black_queen_side = false;

    if (castling != "-")
    {
        for (char c : castling)
        {
            if (c == 'K')
                board.white_king_side = true;
            else if (c == 'Q')
                board.white_queen_side = true;
            else if (c == 'k')
                board.black_king_side = true;
            else if (c == 'q')
                board.black_queen_side = true;
        }
    }
    // en passant target square
    board.en_passant_square = -1;

    if (en_passant != "-")
    {
        board.en_passant_square = name_to_square(en_passant);
    }
}

std::string export_fen(const Board &board)
{
    std::string fen;

    // piece placement
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

    // side to move
    fen += board.white_to_move ? " w " : " b ";

    // castling rights
    std::string castling;

    if (board.white_king_side)
        castling += 'K';
    if (board.white_queen_side)
        castling += 'Q';
    if (board.black_king_side)
        castling += 'k';
    if (board.black_queen_side)
        castling += 'q';

    if (castling.empty())
        castling = "-";

    fen += castling;

    // en passant target square
    fen += " ";

    if (board.en_passant_square == -1)
        fen += "-";
    else
        fen += square_to_name(board.en_passant_square);

    return fen;
}