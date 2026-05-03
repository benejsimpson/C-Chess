#include "core/makemove.hpp"
#include "core/movegen.hpp"
#include <iostream>

//--------------------- Internal helpers

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

//--------------------- Move Making & Undoing

void makeMove(Board &board, Move move, Undo &undo)
{
    // store current board position before move is made
    updateUndoMove(board, move, undo);

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
        undo.capturedPiece = board.squares[capturedSquare];
        removePiece(board, capturedSquare);
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

// gets the data from current board position before a move is made and stores it for easy undo
void updateUndoMove(Board &board, Move move, Undo &undo)
{
    const int from = moveFrom(move);
    const int to = moveTo(move);
    const int flag = moveFlag(move);

    const Piece movedPiece = board.squares[from];

    if (flag == EN_PASSANT)
    {
        undo.capturedPiece = board.squares[board.whiteToMove ? to - 8 : to + 8];
    }
    else
    {
        undo.capturedPiece = board.squares[to];
    }
    
    undo.enPassantSquare = board.enPassantSquare;

    undo.whiteCanKsCastle = board.whiteCanKsCastle;
    undo.whiteCanQsCastle = board.whiteCanQsCastle;
    undo.blackCanKsCastle = board.blackCanKsCastle;
    undo.blackCanQsCastle = board.blackCanQsCastle;

    undo.fullmoveNumber = board.fullmoveNumber;

    undo.hash = board.hash;
}

// gets the data from current undo position and updates the board position when undoing a move
void updateBoardMove(Board &board, Move move, const Undo &undo)
{
    board.enPassantSquare = undo.enPassantSquare;

    board.whiteCanKsCastle = undo.whiteCanKsCastle;
    board.whiteCanQsCastle = undo.whiteCanQsCastle;
    board.blackCanKsCastle = undo.blackCanKsCastle;
    board.blackCanQsCastle = undo.blackCanQsCastle;

    board.fullmoveNumber = undo.fullmoveNumber;

    board.hash = undo.hash;
}

void undoMove(Board &board, Move move, const Undo &undo)
{
    const int from = moveFrom(move);
    const int to = moveTo(move);
    const int flag = moveFlag(move);

    Piece movedPiece = board.squares[to];

    // restore side to move
    board.whiteToMove = !board.whiteToMove;

    // undo promotion
    if (isPromotionFlag(flag))
    {
        movedPiece = board.whiteToMove ? WP : BP;
    }

    removePiece(board, to);
    placePiece(board, from, movedPiece);

    // restore captured piece
    if (flag == EN_PASSANT)
    {
        const int capturedSquare = board.whiteToMove ? to - 8 : to + 8;
        placePiece(board, capturedSquare, undo.capturedPiece);
    }

    else if (undo.capturedPiece != Empty)
    {
        placePiece(board, to, undo.capturedPiece);
    }

    // undo rook move for castling
    if (flag == KING_CASTLE)
    {
        if (board.whiteToMove)
        {
            removePiece(board,5);
            placePiece(board, 7, WR);
        }
        else
        {
            removePiece(board, 61);
            placePiece(board, 63, BR);
        }
    }
    else if (flag == QUEEN_CASTLE)
    {
        if (board.whiteToMove)
        {
            removePiece(board,3);
            placePiece(board, 0, WR);
        }
        else
        {
            removePiece(board, 59);
            placePiece(board, 56, BR);
        }
    }
    updateBoardMove(board, move, undo);
}