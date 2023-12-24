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

/// shifts all bits in a bitboard up one rank
/// \param bitboard U64
/// \return U64
static inline U64 north_one(U64 bitboard) {
    return bitboard >> 8;
}

/// shifts all bits in a bitboard down one rank
/// \param bitboard U64
/// \return U64
static inline U64 south_one(U64 bitboard) {
    return bitboard << 8;
}

// get pawn attack mask
static U64 mask_pawn_attacks(int side, int square) {
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
static U64 mask_single_pawn_pushes(int side, U64 pawns, U64 empty) {
    return side ? (south_one(pawns) & empty) : (north_one(pawns) & empty);
}

/// get double pawn push targets
/// \param side 0 for black, 1 for white
/// \param single_pushes bitboard of available single pawn pushes
/// \param empty bitboard of empty squares
/// \return U64 bitboard of possible pawn pushes
static U64 mask_double_pawn_pushes(int side, U64 single_pushes, U64 empty) {
    // white
    if (!side) {
        // push single pushes forward one more, only if target square is empty and on the 4th rank
        return north_one(single_pushes) & empty & rank_4;
        // black
    } else {
        // push single pushes forward one more, only if target square is empty and on the 5th rank
        return south_one(single_pushes) & empty & rank_5;
    }
}


// bit shifts to get knight attacks
static U64 mask_knight_attacks(int square) {
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
static U64 mask_king_attacks(int square) {
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
static U64 mask_bishop_attacks(int square) {
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
static U64 generate_bishop_attacks(int square, U64 block) {
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
static U64 mask_rook_attacks(int square) {
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
static U64 generate_rook_attacks(int square, U64 block) {
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
static U64 set_occupancy(int index, int bits_in_mask, U64 attack_mask) {
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
static void generate_attack_tables_non_sliding() {
    for (int square = 0; square < 64; square++) { // loop through every square
        // fill table for given square
        pawn_attacks[white][square] = mask_pawn_attacks(white, square);
        pawn_attacks[black][square] = mask_pawn_attacks(black, square);
        knight_attacks[square] = mask_knight_attacks(square);
        king_attacks[square] = mask_king_attacks(square);
    }
}

// fill attack tables for sliding pieces (rook + bishop)
static void generate_attack_tables_sliding(int bishop) {
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

void fill_attack_tables() {
    generate_attack_tables_non_sliding();
    generate_attack_tables_sliding(1);
    generate_attack_tables_sliding(0);
}

/// get bishop attacks using magic number for table lookup
/// \param square int
/// \param occupancy U64 bitboard of occupied squares
/// \return U64 bitboard// get bishop attacks using magic number for table lookup
static inline U64 get_bishop_attacks(int square, U64 occupancy) {
    // get bishop attacks using current board occupancy
    occupancy &= bishop_masks[square]; // just the relevant blockers
    occupancy *= bishop_magic_nums[square]; // multiply by magic number
    occupancy >>= 64 - bishop_relevant_bits[square]; // shift out the garbage, the occupancy index remains

    return bishop_attacks[square][occupancy]; // access pre-calculated table
}

/// get rook attacks using magic number for table lookup
/// \param square int
/// \param occupancy U64 bitboard of occupied squares
/// \return U64 bitboard
static inline U64 get_rook_attacks(int square, U64 occupancy) {
    // get rook attacks using current board occupancy
    occupancy &= rook_masks[square]; // just the relevant blockers
    occupancy *= rook_magic_nums[square]; // multiply by magic number
    occupancy >>= 64 - rook_relevant_bits[square]; // shift out the garbage, the occupancy index remains

    return rook_attacks[square][occupancy]; // access pre-calculated table
}

/// combining results of rook & bishop attack getters
/// \param square int
/// \param occupancy U64 bitboard of occupied squares
/// \return U64 bitboard
static inline U64 get_queen_attacks(int square, U64 occupancy) {
    return get_rook_attacks(square, occupancy) | get_bishop_attacks(square, occupancy);
}

/// function to determine if a given square is attacked by the given side
/// the general idea of this function is to assume one of each piece type is actually currently on the square
/// and then check the attacked squares for enemy pieces
/// basically using rays going outwards from target square
/// \param piece_bitboards bitboards for all the pieces (white and black)
/// \param occupancy bitboard of occupied squares
/// \param square square to check if it is attacked
/// \param by_side (0 for white, 1 for black) side to check if they attack that square
/// \return bool if square is attacked by given side
// source https://www.chessprogramming.org/Square_Attacked_By
static inline bool attacked(const U64 piece_bitboards[12], U64 occupancy, int square, int by_side) {
    // the pieces enum is laid out as pawn, black_pawn, knight, black_knight, etc
    // indexing the bitboard by pawn + by_side will index white pieces if by_side is 0, and black pieces if by_side is 1
    U64 pawns = piece_bitboards[pawn + by_side];
    // we index the pawn_attacks table for the opposite color from the square (by_side^1 will flip the bit)
    // then we check if there are pawns on those squares (of the by_side color)
    if (pawn_attacks[by_side ^ 1][square] & pawns) return true; // return early to save some computation

    // knights
    U64 knights = piece_bitboards[knight + by_side]; // get white knights (for example)
    if (knight_attacks[square] & knights)
        return true; // get attacks from target square, see if there are any white knights there

    // kings (basically same as knights)
    U64 kings = piece_bitboards[king + by_side];
    if (king_attacks[square] & kings) return true;

    // bishops & queens
    U64 bishopsQueens = piece_bitboards[queen + by_side] | piece_bitboards[bishop + by_side];
    // send diagonal ray outwards from squares then & with bitboard containing bishops and queens
    if (get_bishop_attacks(square, occupancy) & bishopsQueens) return true;

    // rooks & queens
    U64 rooksQueens = piece_bitboards[queen + by_side] | piece_bitboards[rook + by_side];
    // send orthogonal ray outwards from squares then & with bitboard containing rooks and queens
    if (get_rook_attacks(square, occupancy) & rooksQueens) return true;

    return false;
}


std::vector<int> pseudo_legal_moves(const U64 occupancy_bitboards[3], U64 piece_bitboards[12], int side) {
    // vector of move pairs (start_pos, end_pos)
    std::vector<int> moves = {};
    int move, source_square, target_square, capture, piece;
    U64 empty = ~occupancy_bitboards[all];

    // NON-SLIDING PIECES (pawn, knight, king)
    // pawn pushes
    U64 single_pawn_pushes = mask_single_pawn_pushes(side, piece_bitboards[pawn + side], empty);
    U64 double_pawn_pushes = mask_double_pawn_pushes(side, single_pawn_pushes, empty);

    // single
    while(single_pawn_pushes) {
        // get source square, target square, and pop
        source_square = get_ls1b_index(single_pawn_pushes);
        target_square = side ? source_square - 8 : source_square + 8;
        pop_bit(single_pawn_pushes, source_square);

        // save move
        move = encode_move(target_square, source_square, side, 0, 0, 0, 0, 0);
        moves.push_back(move);
    }

    // double
    while (double_pawn_pushes) {
        // get source square, target square, and pop
        source_square = get_ls1b_index(double_pawn_pushes);
        target_square = side ? source_square - 16 : source_square + 16;
        pop_bit(double_pawn_pushes, source_square);

        // save move
        move = encode_move(target_square, source_square, side, 0, 0, 1, 0, 0);
        moves.push_back(move);
    }

    // pawn captures
    U64 pawns = piece_bitboards[pawn + side];
    while (pawns) {
        // get source square then pop
        source_square = get_ls1b_index(pawns);
        pop_bit(pawns, source_square);

        // use attack table lookup, & with friendly pieces to disallow self-capture
        U64 attacks = pawn_attacks[side][source_square];// & ~occupancy_bitboards[side];


        // loop through attacked squares
        while (attacks) {
            // get target square
            target_square = get_ls1b_index(attacks);
            pop_bit(attacks, target_square);

            // get capture
            capture = get_bit(occupancy_bitboards[!side], target_square) ? 1 : 0;
            if (capture) {
                // save move
                move = encode_move(source_square, target_square, side, 0, capture, 0, 0, 0);
                moves.push_back(move);
            }
        }
    }

    // knights
    U64 knights = piece_bitboards[knight + side];
    // loop through each knight on the board
    while (knights) {
        // get source square then pop
        source_square = get_ls1b_index(knights);
        pop_bit(knights, source_square);

        // use attack table lookup, & with friendly pieces to disallow self-capture
        U64 attacks = knight_attacks[source_square] & ~occupancy_bitboards[side];

        // loop through attacked squares
        while (attacks) {
            // get target square
            target_square = get_ls1b_index(attacks);
            pop_bit(attacks, target_square);

            // get capture
            capture = get_bit(occupancy_bitboards[!side], target_square) ? 1 : 0;

            // save move
            move = encode_move(source_square, target_square, (knight + side), 0, capture, 0, 0, 0);
            moves.push_back(move);
        }
    }

    // kings
    U64 kings = piece_bitboards[king + side];
    // loop through each king on the board
    while (kings) {
        // get source square then pop
        source_square = get_ls1b_index(kings);
        pop_bit(kings, source_square);

        // use attack table lookup, & with friendly pieces to disallow self-capture
        U64 attacks = king_attacks[source_square] & ~occupancy_bitboards[side];

        // loop through attacked squares
        while (attacks) {
            // get target square
            target_square = get_ls1b_index(attacks);
            pop_bit(attacks, target_square);

            // get capture
            capture = get_bit(occupancy_bitboards[!side], target_square) ? 1 : 0;

            // save move
            move = encode_move(source_square, target_square, (king + side), 0, capture, 0, 0, 0);
            moves.push_back(move);
        }
    }


    // SLIDING PIECES (bishop, rook, queen)
    // bishopsQueens
    U64 bishopsQueens = piece_bitboards[bishop + side] | piece_bitboards[queen + side];
    // loop through each bishop on board
    while (bishopsQueens) {
        // get source square then pop
        source_square = get_ls1b_index(bishopsQueens);
        pop_bit(bishopsQueens, source_square);

        // use attack table lookup, & with friendly pieces to disallow self-capture
        U64 attacks = get_bishop_attacks(source_square, occupancy_bitboards[all]) & ~occupancy_bitboards[side];

        // loop through attacked squares
        while (attacks) {
            // get target square
            target_square = get_ls1b_index(attacks);
            pop_bit(attacks, target_square);

            // get capture
            capture = get_bit(occupancy_bitboards[!side], target_square) ? 1 : 0;

            // get piece (bishop or queen)
            piece = get_bit(piece_bitboards[bishop + side], source_square) ? bishop : queen;

            // save move
            move = encode_move(source_square, target_square, piece, 0, capture, 0, 0, 0);
            moves.push_back(move);
        }
    }

    // rooksQueens
    U64 rooksQueens = piece_bitboards[rook + side] | piece_bitboards[queen + side];
    // loop through each rook on board
    while (rooksQueens) {
        // get source square then pop
        source_square = get_ls1b_index(rooksQueens);
        pop_bit(rooksQueens, source_square);

        // use attack table lookup, & with friendly pieces to disallow self-capture
        U64 attacks = get_rook_attacks(source_square, occupancy_bitboards[all]) & ~occupancy_bitboards[side];

        // loop through attacked squares
        while (attacks) {
            // get target square
            target_square = get_ls1b_index(attacks);
            pop_bit(attacks, target_square);

            // get capture
            capture = get_bit(occupancy_bitboards[!side], target_square) ? 1 : 0;

            // get piece (rook or queen)
            piece = get_bit(piece_bitboards[rook + side], source_square) ? rook : queen;

            // save move
            move = encode_move(source_square, target_square, piece, 0, capture, 0, 0, 0);
            moves.push_back(move);
        }
    }

    return moves;
}


