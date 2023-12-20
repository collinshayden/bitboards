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
    } else {
        return -1;
    }
}

// get pawn attack mask
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


/// get single pawn push targets
/// \param side 0 for black, 1 for white
/// \param pawns bitboard of pawn locations
/// \param empty bitboard of empty squares
/// \return bitboard of possible pawn pushes
U64 mask_single_pawn_pushes(int side, U64 pawns, U64 empty) {
    return side ? (south_one(pawns) & empty) : (north_one(pawns) & empty);
}

/// get double pawn push targets
/// \param side 0 for black, 1 for white
/// \param pawns bitboard of pawn locations
/// \param empty bitboard of empty squares
/// \return U64 bitboard of possible pawn pushes
U64 mask_double_pawn_pushes(int side, U64 pawns, U64 empty) {
    // get single pushes
    U64 single_pushes = mask_single_pawn_pushes(side, pawns, empty);
    if (!side) {
        // push single pushes forward one more, only if target square is empty and on the 4th rank
        return north_one(single_pushes) & empty & rank_4;
    }
    else {
        // push single pushes forward one more, only if target square is empty and on the 5th rank
        return south_one(single_pushes) & empty & rank_5;
    }
}


// bit shifts to get knight attacks
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

// get king attack mask using bit shifts
U64 mask_king_attacks(int square) {
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;

    set_bit(bitboard, square);

    // bit shift to get attacked squares
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
U64 generate_bishop_attacks(int square, U64 block) {
    U64 attacks = 0ULL;

    int rank, file;

    int target_rank = square / 8;
    int target_file = square % 8;

    // mask relevant bishop occupancy bits
    // bottom right
    for (rank = target_rank + 1, file = target_file + 1; rank <= 7 && file <= 7; rank++, file++) {
        attacks |= (1ULL << (rank * 8 + file));
        if ((1ULL << (rank * 8 + file)) & block) break; // if the ray hits a blocker
    }
    // top right
    for (rank = target_rank - 1, file = target_file + 1; rank >= 0 && file <= 7; rank--, file++) {
        attacks |= (1ULL << (rank * 8 + file));
        if ((1ULL << (rank * 8 + file)) & block) break;
    }
    // top left
    for (rank = target_rank - 1, file = target_file - 1; rank >= 0 && file >= 0; rank--, file--) {
        attacks |= (1ULL << (rank * 8 + file));
        if ((1ULL << (rank * 8 + file)) & block) break;
    }
    // bottom left
    for (rank = target_rank + 1, file = target_file - 1; rank <= 7 && file >= 0; rank++, file--) {
        attacks |= (1ULL << (rank * 8 + file));
        if ((1ULL << (rank * 8 + file)) & block) break;
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
U64 generate_rook_attacks(int square, U64 block) {
    U64 attacks = 0ULL;

    int rank, file;

    int target_rank = square / 8;
    int target_file = square % 8;

    // mask relevant rook occupancy bits
    for (rank = target_rank + 1; rank <= 7; rank++) {
        attacks |= (1ULL << (rank * 8 + target_file));
        if ((1ULL << (rank * 8 + target_file)) & block) break; // if the ray hits a blocker
    }
    for (rank = target_rank - 1; rank >= 0; rank--) {
        attacks |= (1ULL << (rank * 8 + target_file));
        if ((1ULL << (rank * 8 + target_file)) & block) break;
    }
    for (file = target_file + 1; file <= 7; file++) {
        attacks |= (1ULL << (target_rank * 8 + file));
        if ((1ULL << (target_rank * 8 + file)) & block) break;
    }
    for (file = target_file - 1; file >= 0; file--) {
        attacks |= (1ULL << (target_rank * 8 + file));
        if ((1ULL << (target_rank * 8 + file)) & block) break;
    }

    return attacks;
}

// set the occupancy for a bitboard
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

// fill attack tables for the non-sliding pieces (pawn, king, knight)
void generate_attack_tables_non_sliding() {
    for (int square = 0; square < 64; square++) { // loop through every square
        // fill table for given square
        pawn_attacks[white][square] = mask_pawn_attacks(white, square);
        pawn_attacks[black][square] = mask_pawn_attacks(black, square);
        knight_attacks[square] = mask_knight_attacks(square);
        king_attacks[square] = mask_king_attacks(square);
    }
}

// fill attack tables for sliding pieces (rook + bishop)
void generate_attack_tables_sliding(int bishop) {
    for (int square = 0; square < 64; square++) {
        // init masks
        bishop_masks[square] = mask_bishop_attacks(square);
        rook_masks[square] = mask_rook_attacks(square);

        // init current mask
        U64 attack_mask = bishop ? bishop_masks[square] : rook_masks[square];

        int relevant_bits_count = count_bits(attack_mask);

        // init occupancy indices
        int occupancy_indices = (1 << relevant_bits_count);

        for (int i = 0; i < occupancy_indices; i++) {

            if (bishop) {
                // current occupancy variation
                U64 occupancy = set_occupancy(i, relevant_bits_count, attack_mask);
                // get the magic index from occupancy, magic number table, and relevant occupancy table
                int magic_index = (occupancy * bishop_magic_nums[square]) >> (64 - bishop_relevant_bits[square]);

                //fill bishop attacks table
                bishop_attacks[square][magic_index] = generate_bishop_attacks(square, occupancy);
            } else { // rook
                // current occupancy variation
                U64 occupancy = set_occupancy(i, relevant_bits_count, attack_mask);
                // get the magic index from occupancy, magic number table, and relevant occupancy table
                int magic_index = (occupancy * rook_magic_nums[square]) >> (64 - rook_relevant_bits[square]);

                //fill rook attacks table
                rook_attacks[square][magic_index] = generate_rook_attacks(square, occupancy);
            }
        }
    }
}

// get bishop attacks using magic number for table lookup
static inline U64 get_bishop_attacks(int square, U64 occupancy) {
    // get bishop attacks using current board occupancy
    occupancy &= bishop_masks[square]; // just the relevant blockers
    occupancy *= bishop_magic_nums[square]; // multiply by magic number
    occupancy >>= 64 - bishop_relevant_bits[square]; // shift out the garbage, the occupancy index remains

    return bishop_attacks[square][occupancy]; // access pre-calculated table
}

// get rook attacks using magic number for table lookup
static inline U64 get_rook_attacks(int square, U64 occupancy) {
    // get rook attacks using current board occupancy
    occupancy &= rook_masks[square]; // just the relevant blockers
    occupancy *= rook_magic_nums[square]; // multiply by magic number
    occupancy >>= 64 - rook_relevant_bits[square]; // shift out the garbage, the occupancy index remains

    return rook_attacks[square][occupancy]; // access pre-calculated table
}

//
//int main() {
//    generate_attack_tables_non_sliding();
//    generate_attack_tables_sliding(1);
//    generate_attack_tables_sliding(0);
//
//    U64 occupancy = 0ULL;
//    set_bit(occupancy, b6);
//    set_bit(occupancy, f2);
//    set_bit(occupancy, b2);
//
//    print_bitboard(get_bishop_attacks(d4, occupancy));
//    return 0;
//}