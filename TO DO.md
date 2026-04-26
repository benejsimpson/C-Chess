# TO DO:
## Piece Square Tables
For each piece type, assign a +/- value relative to the strength of the piece in that position.
- Create PSQT for different styles of gameplay (opening, middlegame, endgame)
  - develop pieces in opening before queen
  - this should stop the AI going for scholars mate every game
- Simulate AI vs AI games to alter PSQT parameters

## Move Ordering
Improve search efficiency by ordering moves with most likely to be a 'good' move prioritised. This should greatly improve the efficiency of alpha-beta pruning.
1. Checks
2. Pawn Promotions
3. Pawn Captures
4. Captures
5. Normal

## Transpositions
Same position can occur via many routes. Create hashes to remove unnecessary compute in previously searched positions.

## Other Parameters
- King safety measure
- Pawn structure
  + for connected pawns
  + for passed pawn
  - for isolated pawn
  - for doubled pawns
  - 
