//
// Created by Hayden Collins on 12/22/23.
//
#include "Board.h"
#include "MoveGeneration.h"


int main() {
    fill_attack_tables();
    Board board;

    std::vector<int> moves = board.get_legal_moves();
    board.print_legal_moves(moves);

    return 0;
}