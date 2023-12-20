//
// Created by Hayden Collins on 12/20/23.
//

#ifndef BITBOARDS_BOARD_H
#define BITBOARDS_BOARD_H

#include "MoveGeneration.h"

class Board {
    U64 white_pawns = 0ULL;
    U64 black_pawns = 0ULL;
    U64 white_knights = 0ULL;
    U64 black_knights = 0ULL;
    U64 white_bishops = 0ULL;
    U64 black_bishops = 0ULL;
    U64 white_rooks = 0ULL;
    U64 black_rooks = 0ULL;
    U64 white_queens = 0ULL;
    U64 black_queens = 0ULL;
    U64 white_kings = 0ULL;
    U64 black_kings = 0ULL;

    //    U64 generate_pseudo_legal_moves(U64 white_pawns, )
public:
    void set_starting_position();
    U64 get_white_occupancy();
    U64 get_black_occupancy();
    U64 get_total_occupancy();
};


#endif //BITBOARDS_BOARD_H
