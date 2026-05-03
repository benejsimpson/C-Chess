#include "core/movegen.hpp"
#include "core/makemove.hpp"
#include "core/board.hpp"
#include "core/move.hpp"

#include <vector>
using namespace std;

// Constants and tables

// bitmask of squares that must be empty for white to castle kingside
static constexpr BitB WHITE_KINGSIDE_CASTLE_EMPTY_SQUARES = 1ULL << 5 | 1ULL << 6;
// bitmask of squares that must be empty for white to castle queenside
static constexpr BitB WHITE_QUEENSIDE_CASTLE_EMPTY_SQUARES = 1ULL << 1 | 1ULL << 2 | 1ULL << 3;
// bitmask of squares that must be empty for black to castle kingside
static constexpr BitB BLACK_KINGSIDE_CASTLE_EMPTY_SQUARES = 1ULL << 61 | 1ULL << 62;
// bitmask of squares that must be empty for black to castle queenside
static constexpr BitB BLACK_QUEENSIDE_CASTLE_EMPTY_SQUARES = 1ULL << 57 | 1ULL << 58 | 1ULL << 59;
// bitmask of squares that must not be attacked for white to castle kingside
static constexpr BitB WHITE_KINGSIDE_CASTLE_ATTACKED_SQUARES = 1ULL << 4 | 1ULL << 5 | 1ULL << 6;
// bitmask of squares that must not be attacked for white to castle queenside
static constexpr BitB WHITE_QUEENSIDE_CASTLE_ATTACKED_SQUARES = 1ULL << 2 | 1ULL << 3 | 1ULL << 4;
// bitmask of squares that must not be attacked for black to castle kingside
static constexpr BitB BLACK_KINGSIDE_CASTLE_ATTACKED_SQUARES = 1ULL << 60 | 1ULL << 61 | 1ULL << 62;
// bitmask of squares that must not be attacked for black to castle queenside
static constexpr BitB BLACK_QUEENSIDE_CASTLE_ATTACKED_SQUARES = 1ULL << 58 | 1ULL << 59 | 1ULL << 60;

// Internal helpers

static void generatePawnMoves(const Board &board, MoveList &moves, int square);
static void generateKnightMoves(const Board &board, MoveList &moves, int square);
static void generateBishopMoves(const Board &board, MoveList &moves, int square);
static void generateRookMoves(const Board &board, MoveList &moves, int square);
static void generateQueenMoves(const Board &board, MoveList &moves, int square);
static void generateKingMoves(const Board &board, MoveList &moves, int square);

static bool isSquareAttackedByPawn(const Board &board, int square, bool by_white);
static bool isSquareAttackedByKnight(const Board &board, int square, bool by_white);
static bool isSquareAttackedByDiagonal(const Board &board, int square, bool by_white);
static bool isSquareAttackedByStraight(const Board &board, int square, bool by_white);
static bool isSquareAttackedByKing(const Board &board, int square, bool by_white);

bool isSquareAttacked(const Board &board, int square, bool by_white);

bool isInCheck(const Board &board, bool white_king);

static bool isLegalPositionAfterMove(const Board &board, const Move &move);

static BitB capturableOpponentOccupancy(const Board &board, bool white);

// Move generation

MoveList generateLegalMoves(const Board &board)
{
    MoveList pseudo_moves = generatePseudoLegalMoves(board);
    MoveList legalMoves;

    for (int i = 0; i < pseudo_moves.count; ++i)
    {
        Move move = pseudo_moves.moves[i];

        if (isLegalPositionAfterMove(board, move))
        {
            legalMoves.add(move);
        }
    }

    return legalMoves;
}

MoveList generatePseudoLegalMoves(const Board &board)
{
    MoveList moves;

    BitB pawns = board.pieceBBs[pieceToBitboardIndex(board.whiteToMove ? WP : BP)];
    while (pawns)
    {
        int from = popLSB(pawns);
        generatePawnMoves(board, moves, from);
    }

    BitB knights = board.pieceBBs[pieceToBitboardIndex(board.whiteToMove ? WN : BN)];
    while (knights)
    {
        int from = popLSB(knights);
        generateKnightMoves(board, moves, from);
    }

    BitB bishops = board.pieceBBs[pieceToBitboardIndex(board.whiteToMove ? WB : BB)];
    while (bishops)
    {
        int from = popLSB(bishops);
        generateBishopMoves(board, moves, from);
    }

    BitB rooks = board.pieceBBs[pieceToBitboardIndex(board.whiteToMove ? WR : BR)];
    while (rooks)
    {
        int from = popLSB(rooks);
        generateRookMoves(board, moves, from);
    }

    BitB queens = board.pieceBBs[pieceToBitboardIndex(board.whiteToMove ? WQ : BQ)];
    while (queens)
    {
        int from = popLSB(queens);
        generateQueenMoves(board, moves, from);
    }

    BitB king = board.pieceBBs[pieceToBitboardIndex(board.whiteToMove ? WK : BK)];
    int from = LsbIndex(king);
    if (from == -1)
        cout << "king not found\n";
    else
        generateKingMoves(board, moves, from);

    return moves;
}

MoveList generateLegalMovesForSquare(const Board &board, const int square)
{
    MoveList moves;
    MoveList all_moves = generateLegalMoves(board);

    for (int i = 0; i < all_moves.count; ++i)
    {
        Move move = all_moves.moves[i];

        if (moveFrom(move) == square)
        {
            moves.add(move);
        }
    }

    return moves;
}

static bool isLegalPositionAfterMove(const Board &board, const Move &move)
{
    Board copy = board;

    bool sideThatMoved = copy.whiteToMove;

    applyMove(copy, move);

    return !isInCheck(copy, sideThatMoved);
}

// Piece move generation helpers

static void quietMove(const int from, const int to, MoveList &moves)
{
    moves.add(createMove(from, to, QUIET));
}

static void captureMove(const int from, const int to, MoveList &moves)
{
    moves.add(createMove(from, to, CAPTURE));
}

static void generatePawnPromotions(const int from, const int to, MoveList &moves)
{
    moves.add(createMove(from, to, Q_PROMO));
    moves.add(createMove(from, to, R_PROMO));
    moves.add(createMove(from, to, B_PROMO));
    moves.add(createMove(from, to, N_PROMO));
}

static void generatePawnMoves(const Board &board, MoveList &moves, int from)
{
    const bool white = board.whiteToMove;
    const int rank = indexToRank(from);
    const int direction = white ? 8 : -8;

    const BitB allOccup = allOccupancyBB(board);
    const BitB captureOccup = capturableOpponentOccupancy(board, white);

    const int oneForward = from + direction;
    const bool promotion_rank = rank == (white ? 6 : 1);

    // Forward move
    if (isValidIndex(oneForward) && !isBitSet(allOccup, oneForward))
    {
        if (promotion_rank)
        {
            generatePawnPromotions(from, oneForward, moves);
        }
        else
        {
            quietMove(from, oneForward, moves);

            const bool startRank = rank == (white ? 1 : 6);
            const int twoForward = from + 2 * direction;

            if (startRank && !isBitSet(allOccup, twoForward))
            {
                moves.add(createMove(from, twoForward, DOUBLE_PAWN));
            }
        }
    }

    // Captures
    BitB attacks = white ? WHITE_PAWN_ATTACKS[from] : BLACK_PAWN_ATTACKS[from];

    while (attacks)
    {
        const int to = popLSB(attacks);

        if (to == board.enPassantSquare)
        {
            moves.add(createMove(from, to, EN_PASSANT));
        }
        else if (isBitSet(captureOccup, to))
        {
            if (promotion_rank)
            {
                generatePawnPromotions(from, to, moves);
            }
            else
            {
                captureMove(from, to, moves);
            }
        }
    }
}

static void generateKnightMoves(const Board &board, MoveList &moves, int from)
{
    const BitB allOccup = allOccupancyBB(board); // bb of all occupied squares
    const bool white = board.whiteToMove;        // piece on start square is a [white] piece
    const BitB captureOccup = capturableOpponentOccupancy(board, white);

    BitB const canMoveToSquares = KNIGHT_ATTACKS[from];
    BitB emptySquaresToMoveTo = canMoveToSquares & ~allOccup;
    BitB squaresWithOpponentPieces = canMoveToSquares & captureOccup;

    while (emptySquaresToMoveTo)
    {
        const int to = popLSB(emptySquaresToMoveTo);
        quietMove(from, to, moves);
    }

    while (squaresWithOpponentPieces)
    {
        const int captureSquare = popLSB(squaresWithOpponentPieces);
        captureMove(from, captureSquare, moves);
    }
}

static void generateBishopMoves(const Board &board, MoveList &moves, int from)
{
    const BitB allOccup = allOccupancyBB(board); // bb of all occupied squares
    const bool white = board.whiteToMove;        // piece on start square is a [white] piece
    const BitB captureOccup = capturableOpponentOccupancy(board, white);

    // loop through each diagonal direction from start square
    for (int i = 0; i < 4; i++)
    {
        int currentSquare = from;
        while (true)
        {
            // get next square index on diagonal
            const int targetSquare = getNextMoveIndex(
                currentSquare,
                DIAGONAL_MOVES[i][0],
                DIAGONAL_MOVES[i][1]);

            if (targetSquare == -1) // if off board -> try another diagonal
                break;

            if (!isBitSet(allOccup, targetSquare)) // empty square -> piece can move here
            {
                quietMove(from, targetSquare, moves);
            }

            else if (isBitSet(captureOccup, targetSquare)) // opponent piece on square -> can capture but no more moves on this diagonal
            {
                captureMove(from, targetSquare, moves);
                break;
            }
            // own piece blocks moves
            else
                break;
            currentSquare = targetSquare;
        }
    }
}

static void generateRookMoves(const Board &board, MoveList &moves, int square)
{
    const BitB allOccup = allOccupancyBB(board); // bb of all occupied squares
    const bool white = board.whiteToMove;        // piece on start square is a [white] piece?
    const BitB captureOccup = capturableOpponentOccupancy(board, white);

    // loop through each straight direction from start square
    for (int i = 0; i < 4; i++)
    {
        int currentSquare = square; // current square being tested
        const int dFile = STRAIGHT_MOVES[i][0];
        const int dRank = STRAIGHT_MOVES[i][1];

        while (true)
        {
            const int targetSquare = getNextMoveIndex(currentSquare, dFile, dRank);

            // stop if off board
            if (targetSquare == -1)
                break;

            if (!isBitSet(allOccup, targetSquare)) // no piece on square -> can make quiet move
            {
                quietMove(square, targetSquare, moves);
            }

            else if (isBitSet(captureOccup, targetSquare)) // opponent piece on square -> can capture
            {
                captureMove(square, targetSquare, moves);
                break;
            }

            // own piece blocks moves
            else
                break;
            currentSquare = targetSquare;
        }
    }
}

static void generateQueenMoves(const Board &board, MoveList &moves, int from)
{
    // Queen = bishop + rook movement
    generateBishopMoves(board, moves, from);
    generateRookMoves(board, moves, from);
}

static void generateKingMoves(const Board &board, MoveList &moves, int from)
{
    const bool white = board.whiteToMove;
    const BitB allOccup = allOccupancyBB(board);
    const BitB captureOccup = capturableOpponentOccupancy(board, white);
    const BitB oppAttacks = white ? board.blackAttacksMask : board.whiteAttacksMask;

    BitB king_moves = KING_ATTACKS[from];

    while (king_moves)
    {
        const int to = popLSB(king_moves);
        if (isBitSet(captureOccup, to) && !isBitSet(oppAttacks, to)) // can capture opponent piece if square not attacked by opponent
            captureMove(from, to, moves);
        else if (!isBitSet(allOccup, to) && !isBitSet(oppAttacks, to)) // can move to empty square if not attacked by opponent
            quietMove(from, to, moves);
    }

    generateKingCastles(board, moves, from);
}

// check / attack helpers

inline bool isSquareAttackedByPawn(const Board &board, int square, bool by_white)
{
    return (
               (by_white ? BLACK_PAWN_ATTACKS[square] : WHITE_PAWN_ATTACKS[square]) & board.pieceBBs[pieceToBitboardIndex(by_white ? WP : BP)]) != EMPTY_BB;
}

inline bool isSquareAttackedByKnight(const Board &board, int square, bool by_white)
{
    return (
        (KNIGHT_ATTACKS[square]
            & board.pieceBBs[pieceToBitboardIndex(by_white ? WN : BN)])
            != EMPTY_BB);
}

inline bool isSquareAttackedByDiagonal(const Board &board, int square, bool by_white)
{
    if (diagonalAttackersBB(board, by_white) == EMPTY_BB)
        return false;

    for (int i = 0; i < 4; i++)
    {
        int currentSquare = square;

        while (true)
        {
            const int targetSquare = getNextMoveIndex(currentSquare, DIAGONAL_MOVES[i][0], DIAGONAL_MOVES[i][1]);

            // stop if off board
            if (targetSquare == -1)
                break;

            if (!isBitSet(allOccupancyBB(board), targetSquare)) // nothing on square -> next
            {
                currentSquare = targetSquare;
                continue;
            }

            if (isBitSet(diagonalAttackersBB(board, by_white), targetSquare))
                return true;
            break;
        }
    }
    return false;
}

inline bool isSquareAttackedByStraight(const Board &board, int square, bool by_white)
{
    if (straightAttackersBB(board, by_white) == EMPTY_BB)
        return false;

    const int directions[4][2] = {{0, -1}, {0, 1}, {1, 0}, {-1, 0}};

    for (int i = 0; i < 4; i++)
    {
        int currentSquare = square;

        while (true)
        {
            const int targetSquare = getNextMoveIndex(currentSquare, directions[i][0], directions[i][1]);

            // stop if off board
            if (targetSquare == -1)
                break;

            if (!isBitSet(allOccupancyBB(board), targetSquare)) // nothing on square -> next
            {
                currentSquare = targetSquare;
                continue;
            }

            if (isBitSet(straightAttackersBB(board, by_white), targetSquare))
                return true;
            break;
        }
    }
    return false;
}

inline bool isSquareAttackedByKing(const Board &board, int square, bool by_white)
{
    return (
               KING_ATTACKS[square] & board.pieceBBs[pieceToBitboardIndex(by_white ? WK : BK)]) != EMPTY_BB;
}

inline bool isInCheck(const Board &board, bool white_king)
{
    int kingSquare = getKingSquareIndex(board, white_king);
    if (kingSquare == -1)
    {
        cout << "ERROR: isInCheck returned getKingSquareIndex = -1\n";
        return true;
    }

    if (white_king)
        return isSquareAttacked(board, kingSquare, false);
    else
        return isSquareAttacked(board, kingSquare, true);
}

bool isSquareAttacked(const Board &board, int square, bool by_white)
{
    if (!isValidIndex(square))
    {
        cout << "INVALID SQUARE in isSquareAttacked : " << square << "\n";
        return false;
    }
    if (isSquareAttackedByPawn(board, square, by_white))
        return true;
    if (isSquareAttackedByKnight(board, square, by_white))
        return true;
    if (isSquareAttackedByDiagonal(board, square, by_white))
        return true;
    if (isSquareAttackedByStraight(board, square, by_white))
        return true;
    if (isSquareAttackedByKing(board, square, by_white))
        return true;
    return false;
}

bool sameMove(const Move &a, const Move &b)
{
    return moveFrom(a) == moveFrom(b) &&
           moveTo(a) == moveTo(b) &&
           moveFlag(a) == moveFlag(b);
}

inline bool isCheckmate(Board &board)
{
    if (!isInCheck(board, board.whiteToMove))
        return false;

    return generateLegalMoves(board).empty();
}

// generates legal castling moves - updates MoveList moves
static void generateKingCastles(const Board &board, MoveList &moves, int from)
{
    const bool white = board.whiteToMove;
    const BitB allOccup = allOccupancyBB(board);
    const BitB rook_bb = board.pieceBBs[pieceToBitboardIndex(white ? WR : BR)];
    const BitB oppAttacks = white ? board.blackAttacksMask : board.whiteAttacksMask;

    // get kings square and squares of rooks on both sides
    const bool rook_can_castle_kingside = isBitSet(rook_bb, (white ? 7 : 63));
    const bool rook_can_castle_queenside = isBitSet(rook_bb, (white ? 0 : 56));

    if (from != (white ? 4 : 60))
        return;

    if (rook_can_castle_kingside && (white ? board.whiteCanKsCastle : board.blackCanKsCastle))
    {
        // kingside castle squares that must be empty between king and rook
        const BitB kingside_squares_mask =
            white
                ? WHITE_KINGSIDE_CASTLE_EMPTY_SQUARES
                : BLACK_KINGSIDE_CASTLE_EMPTY_SQUARES;

        if ((kingside_squares_mask & allOccup) == 0) // squares between king and rook are empty :)
        {
            // kingside castle attacked squares
            const BitB kingside_attacked_squares_mask =
                white
                    ? WHITE_KINGSIDE_CASTLE_ATTACKED_SQUARES
                    : BLACK_KINGSIDE_CASTLE_ATTACKED_SQUARES;

            if ((kingside_attacked_squares_mask & oppAttacks) == 0) // squares king passes through are not attacked
                moves.add(createMove(from, (white ? 6 : 62), KING_CASTLE));
        }
    }

    if (rook_can_castle_queenside && (white ? board.whiteCanQsCastle : board.blackCanQsCastle))
    {
        // queenside castle squares that must be empty between king and rook
        const BitB queenside_squares_mask =
            white
                ? WHITE_QUEENSIDE_CASTLE_EMPTY_SQUARES
                : BLACK_QUEENSIDE_CASTLE_EMPTY_SQUARES;

        if ((queenside_squares_mask & allOccup) == 0) // squares between king and rook are empty :)
        {
            // queenside castle attacked squares
            const BitB queenside_attacked_squares_mask =
                white
                    ? WHITE_QUEENSIDE_CASTLE_ATTACKED_SQUARES
                    : BLACK_QUEENSIDE_CASTLE_ATTACKED_SQUARES;

            if ((queenside_attacked_squares_mask & oppAttacks) == 0) // squares king passes through are not attacked
                moves.add(createMove(from, (white ? 2 : 58), QUEEN_CASTLE));
        }
    }
}

// other helpers

// generates bitboard of all squares [white] attacks - stores in board.[white/black]_attacks
// used for move generation and check detection
// this includes white attacking white pieces
// eg. if a white queen is blocked by a white knight, a bit will still be set at the position of the white knight
// hence, this mask can also be used to detect defended pieces
inline BitB generateAttackMasksForSide(const Board &board, bool white)
{
    BitB attacks = 0;
    const BitB allOccup = allOccupancyBB(board);

    // Pawns
    BitB pawns = board.pieceBBs[pieceToBitboardIndex(white ? WP : BP)];
    while (pawns)
    {
        int from = popLSB(pawns);
        attacks |= white ? WHITE_PAWN_ATTACKS[from] : BLACK_PAWN_ATTACKS[from];
    }

    // Knights
    BitB knights = board.pieceBBs[pieceToBitboardIndex(white ? WN : BN)];
    while (knights)
    {
        int from = popLSB(knights);
        attacks |= KNIGHT_ATTACKS[from];
    }

    // Bishops
    BitB diagonals = diagonalAttackersBB(board, white);
    while (diagonals)
    {
        int from = popLSB(diagonals);
        for (auto &dir : DIAGONAL_MOVES)
        {
            int square = getNextMoveIndex(from, dir[0], dir[1]);

            while (square != -1)
            {
                attacks |= square_mask(square);
                if (isBitSet(allOccup, square))
                    break;
                square = getNextMoveIndex(square, dir[0], dir[1]);
            }
        }
    }

    // Straight pieces (rooks + queens)
    BitB straights = straightAttackersBB(board, white);
    while (straights)
    {
        int from = popLSB(straights);
        for (auto &dir : STRAIGHT_MOVES)
        {
            int square = getNextMoveIndex(from, dir[0], dir[1]);

            while (square != -1)
            {
                attacks |= square_mask(square);
                if (isBitSet(allOccup, square))
                    break;
                square = getNextMoveIndex(square, dir[0], dir[1]);
            }
        }
    }

    BitB king = board.pieceBBs[pieceToBitboardIndex(white ? WK : BK)];
    int from = LsbIndex(king);
    if (from == -1)
        cout << "ERROR generateAttackMasksForSide : king not found\n";
    else
        attacks |= KING_ATTACKS[from];

    return attacks;
}

inline bool isPassedPawn(const Board &board, int square, bool white)
{
    BitB enemy_pawns = white
                           ? board.pieceBBs[pieceToBitboardIndex(BP)]
                           : board.pieceBBs[pieceToBitboardIndex(WP)];

    BitB mask = white
                    ? WHITE_PASSED_PAWN_MASKS[square]
                    : BLACK_PASSED_PAWN_MASKS[square];

    return (enemy_pawns & mask) == EMPTY_BB;
}

// updates board.[white/black]_attacks
// must be called after every move is made or when fen is loaded
inline void updateAttackMasks(Board &board)
{
    board.whiteAttacksMask = generateAttackMasksForSide(board, true);
    board.blackAttacksMask = generateAttackMasksForSide(board, false);
}

// bitboard of opponent pieces that can be captured by the side to move
// does not include opponent king since it cannot be captured
static BitB capturableOpponentOccupancy(const Board &board, bool white)
{
    BitB opp = white ? blackOccupancyBB(board) : whiteOccupancyBB(board);

    // Kings are not capturable pieces.
    opp &= ~board.pieceBBs[pieceToBitboardIndex(white ? BK : WK)];

    return opp;
}
