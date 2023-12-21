//
// Created by Hayden Collins on 12/19/23.
//

#ifndef BITBOARDS_UTILS_H
#define BITBOARDS_UTILS_H

#include <cstdio>

#define U64 unsigned long long

// colors
enum {white, black, all};

// pieces, alternating, so we can do white_pawn + side_to_move to index black pieces
enum {
    white_pawn,
    black_pawn,
    white_knight,
    black_knight,
    white_bishop,
    black_bishop,
    white_rook,
    black_rook,
    white_queen,
    black_queen,
    white_king,
    black_king,
};

// castling rights
enum {
    wk = 1, // 0001 in binary
    wq = 2, // 0010
    bk = 4, // 0100
    bq = 8, // 1000
};

// board squares
enum {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1, no_sq
};

extern const char *square_to_cord[64];

extern const char *unicode_pieces[12];


//Files
const U64 file_A = 0x0101010101010101;
const U64 file_B = 0x0202020202020202;
const U64 file_C = 0x0404040404040404;
const U64 file_D = 0x0808080808080808;
const U64 file_E = 0x1010101010101010;
const U64 file_F = 0x2020202020202020;
const U64 file_G = 0x4040404040404040;
const U64 file_H = 0x8080808080808080;

const U64 not_A_file = ~file_A;
const U64 not_H_file = ~file_H;
const U64 not_AB_file = ~(file_A | file_B);
const U64 not_GH_file = ~(file_G | file_H);


//Ranks
const U64 rank_1 = 0xff00000000000000;
const U64 rank_2 = 0x00ff000000000000;
const U64 rank_3 = 0x0000ff0000000000;
const U64 rank_4 = 0x000000ff00000000;
const U64 rank_5 = 0x00000000ff000000;
const U64 rank_6 = 0x0000000000ff0000;
const U64 rank_7 = 0x000000000000ff00;
const U64 rank_8 = 0x00000000000000ff;


// set/get/pop bit macros
#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define pop_bit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))


void print_bitboard(U64 bitboard);
U64 north_one(U64 bitboard);
U64 south_one(U64 bitboard);

#endif //BITBOARDS_UTILS_H
