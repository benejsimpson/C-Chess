# Core
## bitboard
- Bitboard and helpers
__Constants__
EMPTY_BB = 0ULL
FULL_BB = 1Ull
FILE_A = mask of file 0 (a file)

__Helpers__
using BitB = uint64_t
`BitB square_mask(sq)` : returns BitB with bit set at index sq
`bool isBitSet(BitB, sq)` : returns true if bit is set
`void set_bit(BitB, sq)` : mutates BitB - sets a bit at ind sq
`void clear_bit(BitB, sq)` : mutates BitB - unsets a bit at ind sq
`int count_bits(BitB)` : returns number of bits set
`int LsbIndex(BitB)` : returns first index of set bit
`int popLSB(BitB)` : removes first set bit and returns its index
`BitB [file / rank]_mask(rank)` : returns a mask of a [rank / file]
`print_BitB(BitB)` : debugging

## board
### board.hpp
- Board and helpers
- Piece and helpers
- FEN loading
_includes: bitboards_ 
__Constants__
[TYPE / COLOUR]_MASK
[WHITE / BLACK]_BB_INDS
START_FEN

__Helpers__
int fileRankToIndex(file,rank)
int indexToFile
int indexToRank
bool isValidIndex
bool isValidFileRank
int pieceToBitboardIndex
bool isWhitePiece
bool isBlackPiece
bool isOpponentPieces
Piece getPieceType
bool is_empty_square
bool isPieceOnBoard(Board, Piece) : checks if specific piece exists anywhere on board
bool is_piece_on_square(Board, Piece, sq) : checks if square has a specific piece on it

### board.cpp
_Include: board.hpp, fen.hpp_

__Helpers__
BitB [all / white / black]_occupancy(Board) : returns BitB of all squares occupied by [ ]
int getKingSquareIndex(Board, white?) : returns index of white? king (-1 if not found)
BitB [ straight / diagonal]_attackers(Board, white?) : returns BitB of all squares that white? has Q | [B / R] on

__Functions__
void movePiece(Board, from, to)
    void placePiece(Board, sq, Piece)
    void removePiece(Board, sq)

## move
### move.hpp
[what its for]

_Includes: board.hpp_

using Move = uint16

__Constants__
[TO / FROM / FLAG]_SHIFT
- shifts bit representation of square moved from / to and any move flag to create a 16 bit representation of a move
[TO / FROM / FLAG]_MASK
- uint16 mask for position of bits for representing move

__Structs__
enum MoveFlag : uint8
- generates move flags for type of move:
    QUIET, CAPTURE, [KING / QUEEN]_CASTLE, DOUBLE_PAWN, EN_PASSANT, [N / B / R / Q]_PROMO

__Helpers__
Move createMove(from, to, flag)
- takes int of square moved from/to and int flag
- generates uint16 representation of move
Piece promotionPieceFromFlag(flag, white?)
- takes MoveFlag and white?
- returns Piece promoted to (WQ, BR, ...)
bool isPromotionFlag(flag) : true if flag is a promotion
bool isCapture(Board, move) : true if move is a capture
bool is_castle(move) : true if move is a castle
bool is_en_passant(move) : true if move is en passant
int get_from(move) : gets int square moved from
int get_to(move) : gets int square moved to
int get_flag(move) : gets int MoveFlag
void applyMove(board, move) : IN PROGRESS

## makemove
[what its for]
### makemove.hpp

__Helpers__
void applyMove(Board, Move)

### makemove.cpp
_Include: makemove.hpp_

__Functions__
void applyMove(Board, Move) : main move application
- updates en-passant possibility
- updates castling rights

void removeCastlingRightsForRook(Board, Piece, sq) : removes castling right for colour and side moved
void removeCastlingRightsForKing(Board, Piece) : removes castling right for colour

## movegen
### movegen.hpp
[what its for]

__Constants__
int [KNIGHT / KING / DIAGONAL / STRAIGHT]_MOVES[ ][2]

__Helpers__
int getNextMoveIndex(ind moved from, change in file, change in rank)
- takes index moved from, 
bool isInCheck(Board, white?)
bool isCheckmate(Board)


MoveList generateLegalMoves(Board)
    MoveList generatePseudoLegalMoves(Board)
        void generate_[ piece ]_moves(Board, MoveList)

MoveList generateLegalMovesForSquare(Board, sq)
bool king_can_castle_[king / queen]side(Board, white?)
bool squares_between_king_and_rook_clear
    vec.int squares_to_check_between_king_and_rook
bool sameMove(Move a, Move b)


## fen
### fen.hpp
[what its for]
__Constants__

__Helpers__

## movelist
### movelist.hpp
[what its for]
__Constants__
MAX_MOVES

__Structs__
MoveList
- fixed size array of 16bit move representations

__Helpers__
add(Move move)
- appends 16bit move representation to array
clear()
- clears array

# Engine
## ai
### ai.hpp
[what its for]
__Constants__

__Helpers__

## evaluate
### evaluate.hpp
[what its for]
__Constants__

__Helpers__

## perft
### perft.hpp
[what its for]
__Constants__

__Helpers__


# GUI
## constants
### constants.hpp
[what its for]
__Constants__

__Helpers__

## game
### game.hpp
[what its for]
__Constants__

__Helpers__

## gui_state
### gui_state.hpp
[what its for]
__Constants__

__Helpers__

## gui
### gui.hpp
[what its for]
__Constants__

__Helpers__

## input
### input.hpp
[what its for]
__Constants__

__Helpers__

## renderer
### renderer.hpp
[what its for]
__Constants__

__Helpers__

## textures
### textures.hpp
[what its for]
__Constants__

__Helpers__

## widgets
### widgets.hpp
[what its for]
__Constants__

__Helpers__

