//
// Created by Hayden Collins on 12/19/23.
//

#ifndef BITBOARDS_MOVEGENERATION_H
#define BITBOARDS_MOVEGENERATION_H

#include "utils.h"

// pre calculated attack tables for non-sliding pieces
U64 pawn_attacks[2][64];
U64 knight_attacks[64];
U64 king_attacks[64];

// bishop occupancy bit count for each square
const int bishop_relevant_bits[64] = {
        6, 5, 5, 5, 5, 5, 5, 6,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        6, 5, 5, 5, 5, 5, 5, 6
};

// rook occupancy bit count for each square
const int rook_relevant_bits[64] = {
        12, 11, 11, 11, 11, 11, 11, 12,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        12, 11, 11, 11, 11, 11, 11, 12,
};

//
U64 mask_pawn_attacks(int square, int side);
U64 mask_knight_attacks(int square);
U64 mask_king_attacks(int square);
U64 mask_rook_attacks(int square);
U64 mask_bishop_attacks(int square);
U64 generate_rook_attacks(int square);
U64 generate_bishop_attacks(int square);
U64 set_occupancy(int index, int bits_in_mask, U64 attack_mask);
void generate_attack_tables();
#endif //BITBOARDS_MOVEGENERATION_H
