#include "core/makemove.hpp"
#include "core/movegen.hpp"
#include <iostream>

// Internal helpers

static void removeCastlingRightsForRook(Board &board, int square, Piece rook)
{
    const int oldCastlingIndex = castlingIndex(board); // get castling hash prior to move

    // White rooks
    if (rook == WR)
    {
        if (square == 0)
            board.whiteCanQsCastle = false;
        else if (square == 7)
            board.whiteCanKsCastle = false;
    }

    // Black rooks
    if (rook == BR)
    {
        if (square == 56)
            board.blackCanQsCastle = false;
        else if (square == 63)
            board.blackCanKsCastle = false;
    }

    const int newCastlingIndex = castlingIndex(board); // get castling hash after move
    updateCastlingHash(board, oldCastlingIndex, newCastlingIndex);
}

static void removeCastlingRightsForKing(Board &board, Piece king)
{
    const int oldCastlingIndex = castlingIndex(board); // get castling hash prior to move

    if (king == WK)
    {
        board.whiteCanKsCastle = false;
        board.whiteCanQsCastle = false;
    }
    else if (king == BK)
    {
        board.blackCanKsCastle = false;
        board.blackCanQsCastle = false;
    }

    const int newCastlingIndex = castlingIndex(board); // get castling hash after move
    updateCastlingHash(board, oldCastlingIndex, newCastlingIndex);
}

// Main move application

void applyMove(Board &board, Move move)
{
    const int from = moveFrom(move);
    const int to = moveTo(move);
    const MoveFlag flag = static_cast<MoveFlag>(moveFlag(move));

    const int oldEnPassantSquare = board.enPassantSquare;

    const Piece movedPiece = board.squares[from];
    const Piece capturedPiece = board.squares[to];

    // move removes possible en-passant
    // allow en-passant only when double pawn move
    updateEnPassantHash(board, oldEnPassantSquare, -1);
    board.enPassantSquare = -1;

    // Update castling rights before moving pieces
    // if king moves, that side loses both castling rights
    if (getPieceType(movedPiece) == KING)
    {
        removeCastlingRightsForKing(board, movedPiece);
    }

    // if a rook moves from its original square, that side loses that rook's castling right
    if (getPieceType(movedPiece) == ROOK)
    {
        removeCastlingRightsForRook(board, from, movedPiece);
    }

    // if a rook is captured on its original square, that side loses that castling right
    if (capturedPiece != Empty && getPieceType(capturedPiece) == ROOK)
    {
        removeCastlingRightsForRook(board, to, capturedPiece);
    }

    // CAPTURE
    if (capturedPiece != Empty)
        removePiece(board, to);

    // Handle move by flag

    // en passant capture
    if (flag == EN_PASSANT)
    {
        const int capturedSquare = to + (board.whiteToMove ? -8 : 8);

        movePiece(board, from, to);

        // captured pawn is not on square moved to
        // adjust index to remove captured pawn
        removePiece(board, capturedSquare);

        if (board.squares[capturedSquare] != Empty)
        {
            std::cout << "ERROR: EP pawn still in squares[] at "
                      << capturedSquare << '\n';
        }

        if (isBitSet(allOccupancyBB(board), capturedSquare))
        {
            std::cout << "ERROR: EP pawn still in bitboards at "
                      << capturedSquare << '\n';
        }
    }

    // king-side castle
    else if (flag == KING_CASTLE)
    {
        board.whiteToMove
            ? board.whiteHasKsCastled = true
            : board.blackHasKsCastled = true;

        // move king
        movePiece(board, from, to);

        // move rook as well
        // white : 7 -> 5, black : 63 -> 61
        movePiece(board,
                  (board.whiteToMove ? 7 : 63),
                  (board.whiteToMove ? 5 : 61));
    }

    // queen-side castle
    else if (flag == QUEEN_CASTLE)
    {
        board.whiteToMove
            ? board.whiteHasQsCastled = true
            : board.blackHasQsCastled = true;

        // move king
        movePiece(board, from, to);

        // move rook as well
        // white : 0 -> 3, black : 56 -> 59
        movePiece(board,
                  (board.whiteToMove ? 0 : 56),
                  (board.whiteToMove ? 3 : 59));
    }

    // promotion
    else if (isPromotionFlag(flag))
    {
        removePiece(board, from);
        // place promoted piece in place of piece
        placePiece(board, to, promotionPieceFromFlag(flag, board.whiteToMove));
    }

    // normal move / double pawn move
    else
    {
        movePiece(board, from, to);

        // If a pawn moved 2 squares, record the en passant target square
        if (flag == DOUBLE_PAWN)
        {
            board.enPassantSquare = from + (board.whiteToMove ? 8 : -8);
            updateEnPassantHash(board, -1, board.enPassantSquare);
        }
    }

    // change side to move
    board.whiteToMove = !board.whiteToMove;
    board.hash ^= ZOBRIST.whiteToMoveHash;

    // increment fullmoveNumber after black moves
    if (board.whiteToMove)
    {
        board.fullmoveNumber++;
    }
    // update board.[white/black]_attacks after each move is made
    updateAttackMasks(board);

    if (board.hash != generateHash(board))
    {
        std::cout << "hash generated does not match\n";
        std::cout << "stored:    " << board.hash << '\n';
        std::cout << "generated: " << generateHash(board) << '\n';
        std::cout << "move: " << from << " -> " << to << '\n';
        std::cout << "flag: " << flag << '\n';
        std::cout << "castle index: " << castlingIndex(board) << '\n';
        std::cout << "ep square: " << board.enPassantSquare << '\n';
    }
}
