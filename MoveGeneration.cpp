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
        // if the is_attacked square is on the board, add it to the attacks bitboard
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

    // if the is_attacked square is on the board, add it to the attacks bitboard
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

    // bit shift to get is_attacked squares
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

// generate all squares is_attacked by a bishop, including the edge squares
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

// generate all squares is_attacked by a rook, including the edge squares
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

/// function to determine if a given square is is_attacked by the given side_to_move
/// the general idea of this function is to assume one of each piece type is actually currently on the square
/// and then check the is_attacked squares for enemy pieces
/// basically using rays going outwards from target square
/// \param piece_bitboards bitboards for all the pieces (white and black)
/// \param occupancy bitboard of occupied squares
/// \param square square to check if it is is_attacked
/// \param by_side (0 for white, 1 for black) side_to_move to check if they attack that square
/// \return bool if square is is_attacked by given side_to_move
// source https://www.chessprogramming.org/Square_Attacked_By
inline bool is_attacked(const U64 piece_bitboards[12], U64 occupancy, int square, int by_side) {
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

/// generate a bitboard of all attacked squares by a by_side
/// \param occupancy_bitboards
/// \param piece_bitboards
/// \param by_side
/// \return
U64 attacked_squares(const U64 occupancy_bitboards[3], const U64 piece_bitboards[12], int by_side) {
    // init attacked squares bitboard
    U64 attacked = 0ULL;
    int source_square;

    // NON-SLIDING PIECES (pawns, knights, kings)
    // pawn captures
    U64 pawns = piece_bitboards[pawn + by_side];
    while (pawns) {
        // get source square then pop
        source_square = get_ls1b_index(pawns);
        pop_bit(pawns, source_square);

        // use attack table lookup
        attacked |= pawn_attacks[by_side][source_square];
    }

    // knights
    U64 knights = piece_bitboards[knight + by_side];
    // loop through each knight on the board
    while (knights) {
        // get source square then pop
        source_square = get_ls1b_index(knights);
        pop_bit(knights, source_square);

        // use attack table lookup, & with friendly pieces to disallow self-capture
        attacked |= knight_attacks[source_square];
    }

    // kings
    U64 kings = piece_bitboards[king + by_side];
    // loop through each king on the board
    while (kings) {
        // get source square then pop
        source_square = get_ls1b_index(kings);
        pop_bit(kings, source_square);

        // use attack table lookup, & with friendly pieces to disallow self-capture
        attacked |= king_attacks[source_square];
    }

    // SLIDING PIECES (bishop, rook, queen)
    // bishopsQueens
    U64 bishopsQueens = piece_bitboards[bishop + by_side] | piece_bitboards[queen + by_side];
    // loop through each bishop on board
    while (bishopsQueens) {
        // get source square then pop
        source_square = get_ls1b_index(bishopsQueens);
        pop_bit(bishopsQueens, source_square);

        // use attack table lookup
        attacked |= get_bishop_attacks(source_square, occupancy_bitboards[all]);
    }

    // rooksQueens
    U64 rooksQueens = piece_bitboards[rook + by_side] | piece_bitboards[queen + by_side];
    // loop through each rook on board
    while (rooksQueens) {
        // get source square then pop
        source_square = get_ls1b_index(rooksQueens);
        pop_bit(rooksQueens, source_square);

        // use attack table lookup, & with friendly pieces to disallow self-capture
        attacked |= get_rook_attacks(source_square, occupancy_bitboards[all]) & ~occupancy_bitboards[by_side];
    }

    return attacked;
}

/// get attacked squares by enemy as if our king wasn't on the board
/// this is useful when calculating where a king can move to in the case where the king is a blocker
/// and just using attacked_squares() would allow the king to move along the attacking ray, remaining in check
/// \param occupancy_bitboards
/// \param piece_bitboards
/// \param for_side
/// \return
U64 get_king_danger_squares(U64 occupancy_bitboards[3], U64 piece_bitboards[12], int for_side) {
    int king_square = get_ls1b_index(piece_bitboards[king + for_side]); // assuming only one king

    //remove from piece and occupancy bitboards
    pop_bit(occupancy_bitboards[for_side], king_square);
    pop_bit(occupancy_bitboards[all], king_square);
    pop_bit(piece_bitboards[king + for_side], king_square);

    // calculate squares attacked by opponent without our king
    U64 attacked = attacked_squares(occupancy_bitboards, piece_bitboards, !for_side);

    //add back to piece and occupancy bitboards
    set_bit(occupancy_bitboards[for_side], king_square);
    set_bit(occupancy_bitboards[all], king_square);
    set_bit(piece_bitboards[king + for_side], king_square);

    return attacked;
}


/// get a bitboard of pieces attacking the king
/// \param occupancy_bitboards
/// \param piece_bitboards
/// \param for_side
/// \return
U64 get_king_attackers(U64 occupancy_bitboards[3], U64 piece_bitboards[12], int for_side) {
    // init variables
    U64 attackers = 0ULL;
    int king_square = get_ls1b_index(piece_bitboards[king + for_side]);

    // add attackers to bitboard
    attackers |= knight_attacks[king_square] & piece_bitboards[knight + !for_side];
    attackers |= pawn_attacks[for_side][king_square] & piece_bitboards[pawn + !for_side];
    attackers |= get_bishop_attacks(king_square, occupancy_bitboards[all]) &
                 (piece_bitboards[bishop + !for_side] | piece_bitboards[queen + !for_side]);
    attackers |= get_rook_attacks(king_square, occupancy_bitboards[all]) &
                 (piece_bitboards[rook + !for_side] | piece_bitboards[queen + !for_side]);
    // we don't need to calculate king attackers because a king can never check another king

    return attackers;
}

/// get bitboard of rays from opponent slider pieces to a square (usually king_square)
/// \param square
/// \param opp_slider_pieces
/// \return
U64 get_slider_rays_from_square(int square, U64 opp_slider_pieces) {
    U64 rays = 0ULL;
    rays |= get_bishop_attacks(square, opp_slider_pieces);
    rays |= get_rook_attacks(square, opp_slider_pieces);

    return rays;
}

/// get attack ray between two squares
/// \param from_square
/// \param to_square
/// \param occupancy
/// \return
U64 opp_slider_rays_to_square(int from_square, int to_square, U64 occupancy) {
    U64 rays = 0ULL;
    // if they are on the same file, then use rook attacks
    if (from_square % 8 == to_square % 8) {
        rays |= get_rook_attacks(from_square, occupancy) & get_rook_attacks(to_square, occupancy);
    }
    // if not on same file, use bishop attacks
    else {
        rays |= get_bishop_attacks(from_square, occupancy) & get_bishop_attacks(to_square, occupancy);
    }
    return rays;
}

/// get a bitboard of (absolutely) pinned pieces
/// \param king_square
/// \param opp_slider_pieces[2] bishopQueens, rookQueens
/// \param occupancy[3] occupancy bitboards
/// \return
U64 get_pinned_pieces(int king_square, int for_side, const U64 opp_slider_pieces[2], U64 occupancy[3]) {
    // init variables
    U64 opp_bishop_attack_rays = 0ULL, opp_rook_attack_rays = 0ULL, pinned, king_rays;
    U64 bishopQueens = opp_slider_pieces[0];
    U64 rookQueens = opp_slider_pieces[1];
    int from_square;

    // get attacked squares by bishops
    while (bishopQueens) {
        from_square = get_ls1b_index(bishopQueens);
        // if the bishop/queen is on the same diagonal as the king
        if ((from_square - king_square) % 9 == 0 || (from_square - king_square) % 7 == 0) {
            // calculate rays
            opp_bishop_attack_rays |= get_bishop_attacks(from_square, 0ULL) & get_bishop_attacks(king_square, 0ULL);
        }
        pop_bit(bishopQueens, from_square);
    }

    // get bishop rays from king
    king_rays = get_bishop_attacks(king_square, occupancy[all]);

    // save pieces pinned by bishops
    pinned = opp_bishop_attack_rays & king_rays & occupancy[for_side];

    // get attacked squares by rooks
    while (rookQueens) {
        from_square = get_ls1b_index(rookQueens);
        opp_rook_attack_rays |= get_rook_attacks(from_square, occupancy[all]);
        pop_bit(rookQueens, from_square);
    }

    // get rook rays from king
    king_rays = get_rook_attacks(king_square, occupancy[all]);

    // save pieces pinned by rooks
    pinned |= opp_rook_attack_rays & king_rays & occupancy[for_side];

    return pinned;

}

/// get the legal moves of a pinned piece
/// could probably optimize to remove the redundant code from get_pinned_pieces
/// \param for_side
/// \param opp_slider_pieces
/// \param occupancy
/// \param pinned
/// \return
std::vector<int>
get_pinned_moves(int king_square, int for_side, const U64 opp_slider_pieces[2], const U64 piece_bitboards[12], const U64 occupancy[3],
                 U64 pinned_pieces, U64 ep_bb) {
    U64 opp_bishop_attack_rays = 0ULL, opp_rook_attack_rays = 0ULL, pinned_bishop_rays, pinned_rook_rays, temp_occupancy, target_squares = 0ULL, captures;
    U64 bishopQueens = opp_slider_pieces[0];
    U64 rookQueens = opp_slider_pieces[1];
    std::vector<int> moves;
    int pinned_square, pinned_piece_type = -1, pinner_square, target_square, move;
    int possibly_pinned_pieces[4] = {pawn, bishop, rook, queen};

    while (pinned_pieces) {
        pinned_piece_type = -1;
        pinned_square = get_ls1b_index(pinned_pieces);
        pop_bit(pinned_pieces, pinned_square);

        // determine pinned piece type
        for (int piece_type: possibly_pinned_pieces) {
            if (get_bit(piece_bitboards[piece_type + for_side], pinned_square)) {
                pinned_piece_type = piece_type;
            }
        }

        // if it is a pawn/bishop/rook/queen
        if (pinned_piece_type != -1) {
            // occupancy without the pinned piece
            temp_occupancy = occupancy[all];
            pop_bit(temp_occupancy, pinned_square);

            // get attacked squares by bishops
            while (bishopQueens) {
                pinner_square = get_ls1b_index(bishopQueens);
//                opp_bishop_attack_rays |= get_bishop_attacks(pinner_square, temp_occupancy);
                // if the bishop/queen is on the same diagonal as the king
                if ((pinner_square - king_square) % 9 == 0 || (pinner_square - king_square) % 7 == 0) {
                    // calculate rays
                    opp_bishop_attack_rays |= get_bishop_attacks(pinner_square, 0ULL) & get_bishop_attacks(king_square, 0ULL);
                }

                set_bit(opp_bishop_attack_rays, pinner_square);
                pop_bit(bishopQueens, pinner_square);
            }
            // get attacked squares by rooks
            while (rookQueens) {
                pinner_square = get_ls1b_index(rookQueens);
                if (pinner_square % 8 == king_square % 8 || pinner_square / 8 == king_square / 8) {
                    opp_rook_attack_rays |= get_rook_attacks(pinner_square, temp_occupancy);
                }
                set_bit(opp_rook_attack_rays, pinner_square);
                pop_bit(rookQueens, pinner_square);
            }
            // get rays from pinned piece
            pinned_bishop_rays = get_bishop_attacks(pinned_square, occupancy[all]);
            // get rays from pinned piece
            pinned_rook_rays = get_rook_attacks(pinned_square, occupancy[all]);

            // bishop / queen
            if (pinned_piece_type == bishop || pinned_piece_type == queen) {
                // get target squares along pinned diagonal
                target_squares |= opp_bishop_attack_rays & pinned_bishop_rays & ~occupancy[for_side];
            }
                // pawn captures
            else {
                target_squares |= opp_bishop_attack_rays & occupancy[!for_side] & pawn_attacks[for_side][pinned_square];
                // en passant
                if (ep_bb & opp_bishop_attack_rays & pawn_attacks[for_side][pinned_square]) {
                    target_square = get_ls1b_index(ep_bb);
                    move = encode_move(pinned_square, target_square, pinned_piece_type, 0, 1, 0, 1, 0, !for_side);
                    moves.push_back(move);
                }
            }
            // rook / queen
            if (pinned_piece_type == rook || pinned_piece_type == queen) {
                // get target squares along pinned diagonal
                target_squares |= opp_rook_attack_rays & pinned_rook_rays & ~occupancy[for_side];
            }
                // pawn pushes
            else {
                // calculate pawn push bitboards
                U64 empty = ~occupancy[all];
                U64 single_pawn_pushes = mask_single_pawn_pushes(for_side, piece_bitboards[pawn + for_side], empty);
                U64 double_pawn_pushes = mask_double_pawn_pushes(for_side, single_pawn_pushes, empty);

                // add to target squares
                target_squares |= opp_rook_attack_rays & single_pawn_pushes;
                target_squares |= opp_rook_attack_rays & double_pawn_pushes;
            }
            // loop through target squares and encode in moves

            captures = target_squares & occupancy[!for_side];
            target_squares = target_squares & ~occupancy[!for_side];
            while (target_squares) {
                // get and pop target square
                target_square = get_ls1b_index(target_squares);
                pop_bit(target_squares, target_square);

                move = encode_move(pinned_square, target_square, pinned_piece_type, 0, 0, 0, 0, 0, -1);
                moves.push_back(move);
            }
            while (captures) {
                // get and pop target square
                target_square = get_ls1b_index(captures);
                pop_bit(captures, target_square);

                // get captured piece
                int captured_piece = -1;
                for (int piece_type = !for_side; piece_type < 10; piece_type += 2) {
                    if (get_bit(piece_bitboards[piece_type], target_square)) {
                        captured_piece = piece_type;
                    }
                }

                move = encode_move(pinned_square, target_square, pinned_piece_type, 0, 1, 0, 0, 0, captured_piece);
                moves.push_back(move);
            }
        }
    }
    return moves;
}

/// get the legal moves in current position
/// \param occupancy_bitboards
/// \param piece_bitboards
/// \param for_side
/// \return
// reference https://peterellisjones.com/posts/generating-legal-chess-moves-efficiently/
std::vector<int>
generate_legal_moves(U64 occupancy_bitboards[3], U64 piece_bitboards[12], int for_side, int castling_rights,
                     int ep_sq) {
    // init variables
    std::vector<int> legal_moves = {};
    int move, source_square, target_square, capture, promoted, en_passant, captured_piece = -1;
    U64 opp_sliding_pieces[2] = {(piece_bitboards[bishop + !for_side] | piece_bitboards[queen + !for_side]),
                                 (piece_bitboards[rook + !for_side] | piece_bitboards[queen + !for_side])};
    U64 friendly_sliding_pieces[2] = {(piece_bitboards[bishop + for_side] | piece_bitboards[queen + for_side]),
                                 (piece_bitboards[rook + for_side] | piece_bitboards[queen + for_side])};
    U64 king_danger_bitboard = get_king_danger_squares(occupancy_bitboards, piece_bitboards, for_side);
    U64 king_attackers = get_king_attackers(occupancy_bitboards, piece_bitboards, for_side);
    U64 empty = ~occupancy_bitboards[all];
    // squares we can capture or push to, init as all squares, narrowed when in check
    U64 capture_mask = 0xFFFFFFFFFFFFFFFF;
    U64 push_mask = 0xFFFFFFFFFFFFFFFF;

    // for en passant
    U64 ep_bb = 0ULL;
    if (ep_sq != no_sq)
        set_bit(ep_bb, ep_sq);
    U64 all_pawn_attacks = 0ULL;

    // king (this assumes only one king on board)
    source_square = get_ls1b_index(piece_bitboards[king + for_side]);
    int king_square = source_square;
    // get king moves by table lookup, only save those that aren't attacked or occupied by friendly pieces
    U64 king_moves = king_attacks[source_square] & ~king_danger_bitboard & ~occupancy_bitboards[for_side];
    while (king_moves) {
        // get target and pop
        target_square = get_ls1b_index(king_moves);
        pop_bit(king_moves, target_square);
        // if it is a capture
        if (get_bit(occupancy_bitboards[!for_side], target_square)) {
            // get captured piece
            for (int piece_type = !for_side; piece_type < 12; piece_type += 2) {
                if (get_bit(piece_bitboards[piece_type], target_square)) {
                    capture = 1;
                    captured_piece = piece_type;
                }
            }
        }

        move = encode_move(source_square, target_square, (king + for_side), 0, capture, 0, 0, 0, captured_piece);
        legal_moves.push_back(move);
    }

    // if the number of attackers on the king is > 1, we are in double check.
    // The only legal moves to get out of double check are king moves, so we can exit early
    int num_king_attackers = count_bits(king_attackers);
    if (num_king_attackers > 1) {
        return legal_moves;
    }
        // if there is only one attacker on the king, we have three options:
        // 1. Move the king out of check (already calculated above)
        // 2. Capture the checking piece
        // 3. Block the checking piece (if being checked by a rook, bishop or queen)
    else if (num_king_attackers == 1) {
        // option 2, we can capture the checking piece
        // if en passant is possible, and we are in check, then the pawn must be checking us.
        capture_mask = king_attackers | ep_bb;
        int attacker_square = get_ls1b_index(king_attackers);

        // if the checking piece is a slider
        if (get_bit((opp_sliding_pieces[0] | opp_sliding_pieces[1]), attacker_square)) {
            // option 3, we can block the checking piece
            push_mask = opp_slider_rays_to_square(attacker_square, source_square, occupancy_bitboards[all]);
        }
            // if the checking piece is not a slider
        else {
            // we can't block it
            push_mask = 0ULL; // empty bitboard
        }
    }
        // if king is not in check
        //castling
    else {
        // white
        if (!for_side) {
            U64 opp_attacked_squares = attacked_squares(occupancy_bitboards, piece_bitboards, black);
            // check castling rights
            if (castling_rights & wk) {
                // if the squares between rook and king are empty
                if (!(castling_squares[0] & occupancy_bitboards[all])) {
                    // if the squares that the king crosses are not attacked
                    if (!(castling_squares[0] & opp_attacked_squares)) {
                        // then castling kingside is legal
                        move = encode_move(e1, g1, king, 0, 0, 0, 0, 1, -1);
                        legal_moves.push_back(move);
                    }
                }
            }
            // check castling rights
            if (castling_rights & wq) {
                // if the squares between rook and king are empty
                if (!(castling_squares[1] & occupancy_bitboards[all])) {
                    // if the squares that the king crosses are not attacked
                    if (!(queenside_occupancy[0] & opp_attacked_squares)) {
                        // then castling queenside is legal
                        move = encode_move(e1, c1, king, 0, 0, 0, 0, 1, -1);
                        legal_moves.push_back(move);
                    }
                }
            }
        }
            // black
        else {
            U64 opp_attacked_squares = attacked_squares(occupancy_bitboards, piece_bitboards, white);
            // check castling rights
            if (castling_rights & bk) {
                // if the squares between rook and king are empty
                if (!(castling_squares[2] & occupancy_bitboards[all])) {
                    // if the squares that the king crosses are not attacked
                    if (!(castling_squares[2] & opp_attacked_squares)) {
                        // then castling kingside is legal
                        move = encode_move(e8, g8, king, 0, 0, 0, 0, 1, -1);
                        legal_moves.push_back(move);
                    }
                }
            }
            // check castling rights
            if (castling_rights & bq) {
                // if the squares between rook and king are empty
                if (!(queenside_occupancy[1] & occupancy_bitboards[all])) {
                    // if the squares that the king crosses are not attacked
                    if (!(castling_squares[3] & opp_attacked_squares)) {
                        // then castling queenside is legal
                        move = encode_move(e8, c8, king, 0, 0, 0, 0, 1, -1);
                        legal_moves.push_back(move);
                    }
                }
            }
        }
    }
    // calculate pinned pieces
    U64 pinned_pieces = get_pinned_pieces(source_square, for_side, opp_sliding_pieces, occupancy_bitboards);
    U64 non_pinned_pieces = occupancy_bitboards[for_side] & ~pinned_pieces;
    std::vector<int> moves = get_pinned_moves(king_square, for_side, opp_sliding_pieces, piece_bitboards, occupancy_bitboards,
                                              pinned_pieces, ep_bb);
    legal_moves.insert(legal_moves.end(), moves.begin(), moves.end());
    // moves for the rest of the pieces (non-king, non-pinned, while king not in check)
    // pawn pushes
    U64 pawns = piece_bitboards[pawn + for_side] & non_pinned_pieces;
    U64 single_pawn_pushes = mask_single_pawn_pushes(for_side, pawns, empty);
    U64 double_pawn_pushes = mask_double_pawn_pushes(for_side, single_pawn_pushes, empty);

    // if king is in check, & with push mask to only allow moves that block check
    single_pawn_pushes &= push_mask;
    double_pawn_pushes &= push_mask;

    while (single_pawn_pushes) {
        // get target and source
        target_square = get_ls1b_index(single_pawn_pushes);
        U64 target_bb = 0ULL;
        set_bit(target_bb, target_square);
        source_square = for_side ? target_square - 8 : target_square + 8;
        pop_bit(single_pawn_pushes, target_square);

        // promotion
        if (target_bb & (rank_1 | rank_8)) {
            for (int promote_type = 2 + for_side; promote_type < 10; promote_type += 2) {
                move = encode_move(source_square, target_square, (pawn + for_side), promote_type, 0, 0, 0, 0, -1);
                legal_moves.push_back(move);
            }
        } else {
            move = encode_move(source_square, target_square, (pawn + for_side), 0, 0, 0, 0, 0, -1);
            legal_moves.push_back(move);
        }

    }

    while (double_pawn_pushes) {
        // get target and source
        target_square = get_ls1b_index(double_pawn_pushes);
        source_square = for_side ? target_square - 16 : target_square + 16;
        pop_bit(double_pawn_pushes, target_square);

        move = encode_move(source_square, target_square, (pawn + for_side), 0, 0, 1, 0, 0, -1);
        legal_moves.push_back(move);
    }

    // pawn captures
    while (pawns) {
        // get source square then pop
        source_square = get_ls1b_index(pawns);
        pop_bit(pawns, source_square);

        // use attack table lookup and capture mask to get all attacks
        U64 attacks = pawn_attacks[for_side][source_square] & capture_mask;

        // en passant
        if (ep_bb & attacks & capture_mask) {
            // get square of opp pawn being captured
            target_square = for_side ? ep_sq - 8 : ep_sq + 8;
            king_square = get_ls1b_index(piece_bitboards[king + for_side]);

            // remove both pawns involved from the board
            pop_bit(piece_bitboards[pawn + for_side], source_square);
            pop_bit(piece_bitboards[pawn + !for_side], target_square);
            pop_bit(occupancy_bitboards[for_side], source_square);
            pop_bit(occupancy_bitboards[!for_side], target_square);

            // check if the resulting position has the king in check
            bool king_in_check = is_attacked(piece_bitboards, *occupancy_bitboards, king_square, !for_side);
            // if not, en passant is legal
            if (!king_in_check) {
                move = encode_move(source_square, ep_sq, (pawn + for_side), 0, 1, 0, 1, 0, !for_side);
                legal_moves.push_back(move);
            }

            // add both pawns involved back to the board
            set_bit(piece_bitboards[pawn + for_side], source_square);
            set_bit(piece_bitboards[pawn + !for_side], target_square);
            set_bit(occupancy_bitboards[for_side], source_square);
            set_bit(occupancy_bitboards[!for_side], target_square);
        }

        // for non-en passant, it must be a capture
        attacks &= occupancy_bitboards[!for_side];

        // loop through attacked squares
        while (attacks) {
            // get target square
            target_square = get_ls1b_index(attacks);
            pop_bit(attacks, target_square);

            // get capture
            capture = get_bit(occupancy_bitboards[!for_side], target_square) ? 1 : 0;
            captured_piece = -1;
            if (capture) {
                // save move
                // promotion
                U64 target_bb = 0ULL;
                set_bit(target_bb, target_square);
                for (int piece_type = !for_side; piece_type < 10; piece_type += 2) {
                    if (get_bit(piece_bitboards[piece_type], target_square)) {
                        captured_piece = piece_type;
                    }
                }

                if (target_bb & (rank_1 | rank_8)) {
                    for (int promote_type = 2 + for_side; promote_type < 10; promote_type+=2) {
                        move = encode_move(source_square, target_square, (pawn + for_side), promote_type, capture, 0, 0,
                                           0, captured_piece);
                        legal_moves.push_back(move);
                    }
                }// if not promotion
                else {
                    move = encode_move(source_square, target_square, (pawn + for_side), 0, capture, 0, 0, 0, captured_piece);
                    legal_moves.push_back(move);
                }
            }
        }
    }

    // knights
    U64 knights = piece_bitboards[knight + for_side] & non_pinned_pieces;
    // loop through each knight on the board
    while (knights) {
        // get source square then pop
        source_square = get_ls1b_index(knights);
        pop_bit(knights, source_square);

        // use attack table lookup, & with friendly pieces to disallow self-capture
        U64 attacks = knight_attacks[source_square] & ~occupancy_bitboards[for_side] & (push_mask | capture_mask);
        // loop through attacked squares
        while (attacks) {
            // get target square
            target_square = get_ls1b_index(attacks);
            pop_bit(attacks, target_square);

            // get capture
            capture = get_bit(occupancy_bitboards[!for_side], target_square) ? 1 : 0;
            captured_piece = -1;
            if (capture) {
                for (int piece_type = !for_side; piece_type < 10; piece_type += 2) {
                    if (get_bit(piece_bitboards[piece_type], target_square)) {
                        captured_piece = piece_type;
                    }
                }
            }
            // save move
            move = encode_move(source_square, target_square, (knight + for_side), 0, capture, 0, 0, 0, captured_piece);
            legal_moves.push_back(move);

        }
    }

    // SLIDING PIECES (bishop, rook, queen)
    // bishopsQueens
    U64 bishopsQueens = (piece_bitboards[bishop + for_side] | piece_bitboards[queen + for_side]) & non_pinned_pieces;
    // loop through each bishop on board
    while (bishopsQueens) {
        // get source square then pop
        source_square = get_ls1b_index(bishopsQueens);
        pop_bit(bishopsQueens, source_square);

        // use attack table lookup, & with friendly pieces to disallow self-capture
        U64 attacks = get_bishop_attacks(source_square, occupancy_bitboards[all]) & ~occupancy_bitboards[for_side] &
                      (push_mask | capture_mask);

        // loop through attacked squares
        while (attacks) {
            // get target square
            target_square = get_ls1b_index(attacks);
            pop_bit(attacks, target_square);

            // get capture
            capture = get_bit(occupancy_bitboards[!for_side], target_square) ? 1 : 0;
            captured_piece = -1;
            if (capture) {
                for (int piece_type = !for_side; piece_type < 10; piece_type += 2) {
                    if (get_bit(piece_bitboards[piece_type], target_square)) {
                        captured_piece = piece_type;
                    }
                }
            }

            // get piece (bishop or queen)
            int piece = get_bit(piece_bitboards[bishop + for_side], source_square) ? bishop : queen;

            // save move
            // TODO piece should be piece + for_side
            move = encode_move(source_square, target_square, piece, 0, capture, 0, 0, 0, captured_piece);
            legal_moves.push_back(move);
        }
    }

    // rooksQueens
    U64 rooksQueens = (piece_bitboards[rook + for_side] | piece_bitboards[queen + for_side]) & non_pinned_pieces;
    // loop through each rook on board
    while (rooksQueens) {
        // get source square then pop
        source_square = get_ls1b_index(rooksQueens);
        pop_bit(rooksQueens, source_square);

        // use attack table lookup, & with friendly pieces to disallow self-capture
        U64 attacks = get_rook_attacks(source_square, occupancy_bitboards[all]) & ~occupancy_bitboards[for_side] &
                      (push_mask | capture_mask);

        // loop through attacked squares
        while (attacks) {
            // get target square
            target_square = get_ls1b_index(attacks);
            pop_bit(attacks, target_square);

            // get capture
            capture = get_bit(occupancy_bitboards[!for_side], target_square) ? 1 : 0;
            captured_piece = -1;
            if (capture) {
                for (int piece_type = !for_side; piece_type < 10; piece_type += 2) {
                    if (get_bit(piece_bitboards[piece_type], target_square)) {
                        captured_piece = piece_type;
                    }
                }
            }

            // get piece (rook or queen)
            int piece = get_bit(piece_bitboards[rook + for_side], source_square) ? rook : queen;

            // save move
            move = encode_move(source_square, target_square, piece, 0, capture, 0, 0, 0, captured_piece);
            legal_moves.push_back(move);
        }
    }
    return legal_moves;
}

