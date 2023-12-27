//
// Created by Hayden Collins on 12/22/23.
//
#include "Board.h"
#include "MoveGeneration.h"

int main() {
    fill_attack_tables();
    Board board;

    U64 white_king = 0ULL;
    set_bit(white_king, e1);

    U64 white_knight = 0ULL;
    set_bit(white_knight, b4);

    U64 black_bishop = 0ULL;
    set_bit(black_bishop, a5);

    U64 black_king = 0ULL;
    set_bit(black_king, e8);

    U64 piece_bitboards[12] = {0ULL, 0ULL, white_knight, 0ULL,
                               0ULL, black_bishop, 0ULL, 0ULL,
                               0ULL, 0ULL, white_king, black_king,};

    U64 occupancy_white = (white_king | white_knight);
    U64 occupancy_black = (black_king | black_bishop);
    U64 occupancy[3] = {occupancy_white, occupancy_black, (occupancy_white | occupancy_black)};
    U64 slider_pieces[2] = {black_bishop, 0ULL};
    U64 pinned = get_pinned_pieces(e1, white, slider_pieces, occupancy);

    print_bitboard(pinned);


    return 0;
}