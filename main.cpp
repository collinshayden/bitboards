//
// Created by Hayden Collins on 12/22/23.
//
#include "Board.h"
#include "MoveGeneration.h"

int main() {
    fill_attack_tables();
    Board board;
    std::vector<int> legal_moves = board.legal_moves();

    for (int legal_move : legal_moves) {
        printf("%s%s\n", square_to_cord[get_move_source(legal_move)], square_to_cord[get_move_target(legal_move)]);
    }


    return 0;
}