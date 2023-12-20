//
// Created by Hayden Collins on 12/20/23.
//

#include "Board.h"

void Board::set_starting_position() {
    // set white pawns
    for (int square = a2; square <= h2; square++) {
        set_bit(white_pawns, square);
    }
    // set black pawns
    for (int square = a7; square <= h7; square++) {
        set_bit(black_pawns, square);
    }

    // set white pieces
    set_bit(white_rooks, a1);
    set_bit(white_rooks, h1);
    set_bit(white_knights, b1);
    set_bit(white_knights, g1);
    set_bit(white_bishops, c1);
    set_bit(white_bishops, f1);
    set_bit(white_queens, d1);
    set_bit(white_kings, e1);

    // set black pieces
    set_bit(black_rooks, a8);
    set_bit(black_rooks, h8);
    set_bit(black_knights, b8);
    set_bit(black_knights, g8);
    set_bit(black_bishops, c8);
    set_bit(black_bishops, f8);
    set_bit(black_queens, d8);
    set_bit(black_kings, e8);
}

// get all squares occupied by a white piece
U64 Board::get_white_occupancy() {
    return white_pawns | white_knights | white_bishops | white_rooks | white_queens | white_kings;
}

// get all squares occupied by a black piece
U64 Board::get_black_occupancy() {
    return black_pawns | black_knights | black_bishops | black_rooks | black_queens | black_kings;
}

// get all occupied squares
U64 Board::get_total_occupancy() {
    return get_white_occupancy() | get_black_occupancy();
}

int main() {
    Board board;
    board.set_starting_position();

    U64 bitboard = board.get_total_occupancy();
    print_bitboard(bitboard);
    return 0;
}
