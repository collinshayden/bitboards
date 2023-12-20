//
// Created by Hayden Collins on 12/19/23.
//

#ifndef BITBOARDS_UTILS_H
#define BITBOARDS_UTILS_H

#include <cstdio>

#define U64 unsigned long long

// colors
enum {white, black};

// board squares
enum {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1
};

extern const char *square_to_cord[64];

//Files
const U64 A = 0x0101010101010101;
const U64 B = 0x0202020202020202;
const U64 C = 0x0404040404040404;
const U64 D = 0x0808080808080808;
const U64 E = 0x1010101010101010;
const U64 F = 0x2020202020202020;
const U64 G = 0x4040404040404040;
const U64 H = 0x8080808080808080;

const U64 not_A_file = ~A;
const U64 not_H_file = ~H;
const U64 not_AB_file = ~(A | B);
const U64 not_GH_file = ~(G | H);


//Ranks
const U64 R1 = 0xff00000000000000;
const U64 R2 = 0x00ff000000000000;
const U64 R3 = 0x0000ff0000000000;
const U64 R4 = 0x000000ff00000000;
const U64 R5 = 0x00000000ff000000;
const U64 R6 = 0x0000000000ff0000;
const U64 R7 = 0x000000000000ff00;
const U64 R8 = 0x00000000000000ff;


// get/set/pop bit macros
#define get_bit(bitboard, square) (bitboard & (1ULL << square))
#define set_bit(bitboard, square) (bitboard |= (1ULL << square))
#define pop_bit(bitboard, square) (get_bit(bitboard, square) ? bitboard ^= (1ULL << square) : 0)


void print_bitboard(U64 bitboard);
U64 north_one(U64 bitboard);
U64 south_one(U64 bitboard);

#endif //BITBOARDS_UTILS_H
