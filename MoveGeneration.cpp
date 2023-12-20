//
// Created by Hayden Collins on 12/19/23.
//

#include "MoveGeneration.h"

// count the number of bits in a bitboard
static inline int count_bits(U64 bitboard) {
    int count = 0;

    while (bitboard) {
        count++;
        bitboard &= bitboard - 1;  // reset least significant 1st bit
    }
    return count;
}

// get least significant 1st bit (ls1b) index
static inline int get_ls1b_index(U64 bitboard) {
    if (bitboard) {
        // count bits before ls1b
        return count_bits((bitboard & -bitboard) - 1);
    }
    else {
        return -1;
    }
}

/// generate pawn attacks
/// \param square origin square of pawn
/// \param side 0 for white, 1 for black
/// \return bitboard of attacked squares for the pawn
U64 mask_pawn_attacks(int side, int square) {
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;

    set_bit(bitboard, square);

    // white
    if (!side) {
        // if the attacked square is on the board, add it to the attacks bitboard
        if ((bitboard >> 7) & not_A_file) attacks |= (bitboard >> 7);
        if ((bitboard >> 9) & not_H_file) attacks |= (bitboard >> 9);
    }
        //black
    else {
        if ((bitboard << 7) & not_H_file) attacks |= (bitboard << 7);
        if ((bitboard << 9) & not_A_file) attacks |= (bitboard << 9);
    }
    return attacks;
}

U64 mask_knight_attacks(int square) {
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;

    set_bit(bitboard, square);

    // if the attacked square is on the board, add it to the attacks bitboard
    if ((bitboard << 17) & not_A_file) attacks |= (bitboard << 17);
    if ((bitboard >> 17) & not_H_file) attacks |= (bitboard >> 17);
    if ((bitboard << 10) & not_AB_file) attacks |= (bitboard << 10);
    if ((bitboard >> 10) & not_GH_file) attacks |= (bitboard >> 10);
    if ((bitboard >> 6) & not_AB_file) attacks |= (bitboard >> 6);
    if ((bitboard << 6) & not_GH_file) attacks |= (bitboard << 6);
    if ((bitboard >> 15) & not_A_file) attacks |= (bitboard >> 15);
    if ((bitboard << 15) & not_H_file) attacks |= (bitboard << 15);

    return attacks;
}

U64 mask_king_attacks(int square) {
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;

    set_bit(bitboard, square);

    if ((bitboard >> 1) & not_H_file) attacks |= (bitboard >> 1);
    if ((bitboard << 1) & not_A_file) attacks |= (bitboard << 1);
    if ((bitboard >> 7) & not_A_file) attacks |= (bitboard >> 7);
    if ((bitboard >> 9) & not_H_file) attacks |= (bitboard >> 9);
    if ((bitboard << 7) & not_H_file) attacks |= (bitboard << 7);
    if ((bitboard << 9) & not_A_file) attacks |= (bitboard << 9);
    if (bitboard << 8) attacks |= (bitboard << 8);
    if (bitboard >> 8) attacks |= (bitboard >> 8);

    return attacks;
}

// generate occupancy squares for bishop moves
// basically get all the squares the bishop attacks, not including the edge of the board
U64 mask_bishop_attacks(int square) {
    U64 attacks = 0ULL;

    int rank, file;

    int target_rank = square / 8;
    int target_file = square % 8;

    // mask relevant bishop occupancy bits
    // bottom right
    for (rank = target_rank + 1, file = target_file + 1; rank <= 6 && file <= 6; rank++, file++) {
        attacks |= (1ULL << (rank * 8 + file));
    }
    // top right
    for (rank = target_rank - 1, file = target_file + 1; rank >= 1 && file <= 6; rank--, file++) {
        attacks |= (1ULL << (rank * 8 + file));
    }
    // top left
    for (rank = target_rank - 1, file = target_file - 1; rank >= 1 && file >= 1; rank--, file--) {
        attacks |= (1ULL << (rank * 8 + file));
    }
    // bottom left
    for (rank = target_rank + 1, file = target_file - 1; rank <= 6 && file >= 1; rank++, file--) {
        attacks |= (1ULL << (rank * 8 + file));
    }

    return attacks;
}

// generate all squares attacked by a bishop, including the edge squares
U64 generate_bishop_attacks(int square) {
    U64 attacks = 0ULL;

    int rank, file;

    int target_rank = square / 8;
    int target_file = square % 8;

    // mask relevant bishop occupancy bits
    // bottom right
    for (rank = target_rank + 1, file = target_file + 1; rank <= 7 && file <= 7; rank++, file++) {
        attacks |= (1ULL << (rank * 8 + file));
    }
    // top right
    for (rank = target_rank - 1, file = target_file + 1; rank >= 0 && file <= 7; rank--, file++) {
        attacks |= (1ULL << (rank * 8 + file));
    }
    // top left
    for (rank = target_rank - 1, file = target_file - 1; rank >= 0 && file >= 0; rank--, file--) {
        attacks |= (1ULL << (rank * 8 + file));
    }
    // bottom left
    for (rank = target_rank + 1, file = target_file - 1; rank <= 7 && file >= 0; rank++, file--) {
        attacks |= (1ULL << (rank * 8 + file));
    }

    return attacks;
}


// generate occupancy squares for rook moves
// basically get all the squares the rook attacks, not including the edge of the board
U64 mask_rook_attacks(int square) {
    U64 attacks = 0ULL;

    int rank, file;

    int target_rank = square / 8;
    int target_file = square % 8;

    // mask relevant rook occupancy bits
    for (rank = target_rank + 1; rank <= 6; rank++) attacks |= (1ULL << (rank * 8 + target_file));
    for (rank = target_rank - 1; rank >= 1; rank--) attacks |= (1ULL << (rank * 8 + target_file));
    for (file = target_file + 1; file <= 6; file++) attacks |= (1ULL << (target_rank * 8 + file));
    for (file = target_file - 1; file >= 1; file--) attacks |= (1ULL << (target_rank * 8 + file));

    return attacks;
}

// generate all squares attacked by a rook, including the edge squares
U64 generate_rook_attacks(int square) {
    U64 attacks = 0ULL;

    int rank, file;

    int target_rank = square / 8;
    int target_file = square % 8;

    // mask relevant rook occupancy bits
    for (rank = target_rank + 1; rank <= 7; rank++) attacks |= (1ULL << (rank * 8 + target_file));
    for (rank = target_rank - 1; rank >= 0; rank--) attacks |= (1ULL << (rank * 8 + target_file));
    for (file = target_file + 1; file <= 7; file++) attacks |= (1ULL << (target_rank * 8 + file));
    for (file = target_file - 1; file >= 0; file--) attacks |= (1ULL << (target_rank * 8 + file));

    return attacks;
}

U64 set_occupancy(int index, int bits_in_mask, U64 attack_mask) {
    U64 occupancy = 0ULL;

    for (int i = 0; i < bits_in_mask; i++) {
        // get ls1b
        int square = get_ls1b_index(attack_mask);

        // pop ls1b
        pop_bit(attack_mask, square);

        // if occupancy is on board
        if (index & (1 << i)) {
            // populate occupancy bitboard
            occupancy |= (1ULL << square);
        }
    }
    return occupancy;
 }


void generate_attack_tables() {
    for (int square = 0; square < 64; square++) {
        pawn_attacks[white][square] = mask_pawn_attacks(white, square);
        pawn_attacks[black][square] = mask_pawn_attacks(black, square);
        knight_attacks[square] = mask_knight_attacks(square);
        king_attacks[square] = mask_king_attacks(square);
    }
}


int main() {
    generate_attack_tables();

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            printf("%d, ", count_bits(mask_rook_attacks(square)));
        }
        printf("\n");
    }

    return 0;
}