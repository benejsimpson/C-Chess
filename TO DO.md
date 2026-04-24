# TO DO:

_msys_: g++ src/app/main.cpp -o output/app.exe -Iinclude -I/ucrt64/include -L/ucrt64/lib -lsfml-main -lsfml-graphics -lsfml-window -lsfml-system
./output/app.exe

## Castling - DONE
add legal castling moves in `src/core/movegen.cpp`

in `generate_king_moves()`

Need to check:
- castling rights are still available
- squares between king and rook are empty
- king not in check
- squares between king and castle position are not attacked
- castled king not in check

## FEN updates for castling - DONE
update `src/core/fen.cpp`to preserve castling rights

## En passant
update `src/core/movegen.cpp` to generate en-passant and validate legality

## FEN updates for en passant
update `src/core/fen.cpp` to support en-passant

## Start using Bitboards more!
Add:
- `BitBoard white_occupancy`
- `BitBoard black_occupancy`
- `BitBoard all_occupancy`

Following structure for piece / board lookup could follow:
what piece on sq? -> `board.squares[sq]`
is sq empty? : `board.squares[sq] == Empty` -> `bb.all_occupancy[sq] == 0`
where are all white knights -> `bb.for_piece_type[WN_ind]`
check if specific piece exists on sq : `bb.for_each_piece[Piece_ind][sq] == 1`
