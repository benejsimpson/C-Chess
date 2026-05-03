#include "core/utils.hpp"
#include "core/fen.hpp"
#include "core/board.hpp"
#include "engine/zobrist.hpp"

void loadFEN(Board &board, const std::string &fen)
{
    clearBoard(board);

    std::string placement = "";
    std::string active_colour = "w";
    std::string castling = "-";
    std::string en_passant = "-";

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
            // IMPORTANT: no std::size_t here
            third_space = fen.find(' ', second_space + 1);

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
        {
            break;
        }

        const Piece piece = charToPiece(c);

        if (piece != Empty)
        {
            int square = rank * 8 + file;
            placePiece(board, square, piece);
        }

        file++;
    }

    // side to move
    board.whiteToMove = (active_colour != "b");

    // reset castling rights
    board.whiteCanKsCastle = false;
    board.whiteCanQsCastle = false;
    board.blackCanKsCastle = false;
    board.blackCanQsCastle = false;

    if (castling != "-")
    {
        for (char c : castling)
        {
            if (c == 'K')
                board.whiteCanKsCastle = true;
            else if (c == 'Q')
                board.whiteCanQsCastle = true;
            else if (c == 'k')
                board.blackCanKsCastle = true;
            else if (c == 'q')
                board.blackCanQsCastle = true;
        }
    }

    // en passant target square
    board.enPassantSquare = -1;

    if (en_passant != "-")
    {
        board.enPassantSquare = nameToSquare(en_passant);
    }
    updateAttackMasks(board);
    board.hash = generateHash(board);
    board.positionHistory.push_back(board.hash);
}

std::string exportFEN(const Board &board)
{
    std::string fen;

    // piece placement
    for (int rank = 7; rank >= 0; rank--)
    {
        int empty_count = 0;

        for (int file = 0; file < 8; file++)
        {
            const Piece piece = board.squares[fileRankToIndex(file, rank)];

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

            fen += pieceToChar(piece);
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
    fen += board.whiteToMove ? " w " : " b ";

    // castling rights
    std::string castling;

    if (board.whiteCanKsCastle)
        castling += 'K';
    if (board.whiteCanQsCastle)
        castling += 'Q';
    if (board.blackCanKsCastle)
        castling += 'k';
    if (board.blackCanQsCastle)
        castling += 'q';

    if (castling.empty())
        castling = "-";

    fen += castling;

    // en passant target square
    fen += " ";

    if (board.enPassantSquare == -1)
        fen += "-";
    else
        fen += squareToName(board.enPassantSquare);

    return fen;
}
