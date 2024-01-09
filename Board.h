//
// Created by Hayden Collins on 12/20/23.
//

#ifndef BITBOARDS_BOARD_H
#define BITBOARDS_BOARD_H

#include "utils.h"
#include "vector"
#include <string>
#include <sstream>

class Board {
public:
    // starting position
    // pawn/knight/bishop/rook/queen/king, alternating pawn/black_pawn etc.
    U64 piece_bitboards[12] = {0x00ff000000000000, 0x000000000000ff00, 0x4200000000000000, 0x0000000000000042,
                               0x2400000000000000, 0x0000000000000024, 0x8100000000000000, 0x0000000000000081,
                               0x0800000000000000, 0x0000000000000008, 0x1000000000000000, 0x0000000000000010,};
    // occupancies, white, black, both
    U64 occupancy_bitboards[3] = {0xffff000000000000, 0x000000000000ffff, 0xffff00000000ffff,};

    int side_to_move = 0; // 0 white, 1 black

    int enpassant = no_sq;

    uint8_t castling_rights = 0xf;

    int half_move = 0;

    int full_move = 1;

    void load_FEN(const std::string& FEN);

    void makeMove(int move);
    void undoMove(int move);

    std::vector<int> get_legal_moves();

    void print_board();

    void print_legal_moves(const std::vector<int>& legal_moves);

};


#endif //BITBOARDS_BOARD_H
