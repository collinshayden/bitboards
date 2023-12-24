//
// Created by Hayden Collins on 12/22/23.
//
#include "Board.h"
#include "MoveGeneration.h"

int main() {
    fill_attack_tables();
    Board board;
    board.legal_moves();

    return 0;
}