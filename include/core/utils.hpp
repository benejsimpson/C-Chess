#pragma once
#include <iostream>
#include <cstdint>
#include <string>
#include <array>

// Square helpers
// file: 0..7 for a..h
// rank: 0..7 for ranks 1..8

// returns int value of file : a..h -> 0..7
// must be lowercase from a,b,c,d,e,f,g,h
inline constexpr int charFileToInt(char file)
{
    return file - 'a';
}

inline constexpr char intFileToChar(int file)
{
    return 'a' + file;
}

// converts integer file/rank to board index
inline constexpr int fileRankToIndex(int file, int rank)
{
    return (rank * 8) + file;
}

// returns the file that board index is on
inline constexpr int indexToFile(int index)
{
    return index % 8;
}

// returns the rank that board index is on
inline constexpr int indexToRank(int index)
{
    return index / 8;
}

inline constexpr bool isValidIndex(int index)
{
    return 0 <= index && index < 64;
}

inline constexpr bool isValidFileRank(int file, int rank)
{
    return 0 <= file && file <= 7 &&
           0 <= rank && rank <= 7;
}

inline std::string squareToName(int square)
{
    if (!isValidIndex(square))
        return "-";

    int r = indexToRank(square);
    char f = intFileToChar(indexToFile(square));

    return std::string(1, f) + std::to_string(r + 1);
}

inline int nameToSquare(const std::string &name)
{
    if (name.size() != 2)
        return -1;

    char fc = name[0];
    char rc = name[1];

    int f = charFileToInt(fc);
    int r = rc - '1';

    int sq = fileRankToIndex(f, r);

    if (!isValidIndex(sq))
        return -1;

    return sq;
}

// 1. index moving from
// 2. change in file
// 3. change in rank
// -> index moved to : -1 if not valid
inline constexpr int getNextMoveIndex(int from, int d_file, int d_rank)
{
    const int to_rank = indexToRank(from) + d_rank;
    const int to_file = indexToFile(from) + d_file;

    if (!isValidFileRank(to_file, to_rank))
        return -1;

    return from + (8 * d_rank) + d_file;
}