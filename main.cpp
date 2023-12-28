//
// Created by Hayden Collins on 12/22/23.
//
#include "Board.h"
#include "MoveGeneration.h"

int main() {
    fill_attack_tables();
    Board board;

    U64 white_king = 0ULL;
    set_bit(white_king, d3);

    U64 white_pawn = 0ULL;
    set_bit(white_pawn, d4);

    U64 black_bishop = 0ULL;
    set_bit(black_bishop, c5);

    U64 black_other = 0ULL;
    set_bit(black_other, e5);

    U64 black_rook = 0ULL;
    set_bit(black_rook, d7);

    U64 black_king = 0ULL;
    set_bit(black_king, e8);

    U64 piece_bitboards[12] = {white_pawn, 0ULL, 0ULL, 0ULL,
                               0ULL,0ULL , 0ULL, 0ULL,
                               0ULL, black_rook, white_king, black_king,};

    U64 occupancy_white = (white_king | white_pawn);
    U64 occupancy_black = (black_king | black_bishop | black_rook);
    U64 occupancy[3] = {occupancy_white, occupancy_black, (occupancy_white | occupancy_black)};
    U64 slider_pieces[2] = {black_bishop, 0ULL};

    std::vector<int> moves = get_legal_moves(occupancy, piece_bitboards, white);

    for (int move : moves) {
        printf("%s%s capture: %d\n", square_to_cord[get_move_source(move)], square_to_cord[get_move_target(move)],
               get_move_capture(move));
    }

    return 0;
}