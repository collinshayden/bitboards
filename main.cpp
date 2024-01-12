//
// Created by Hayden Collins on 12/22/23.
//
#include "Board.h"
#include "MoveGeneration.h"


U64 perft(int depth, Board &board) {
    U64 nodes = 0ULL;

    if (depth == 0) {
        return 1ULL;
    }

    std::vector<int> legal_moves = board.get_legal_moves();
    for (int move : legal_moves) {

    }
}

int main() {
    fill_attack_tables();
    Board board;

//    board.load_FEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    board.load_FEN("r1bqk2r/pppp1ppp/2n2n2/4p3/1b2P3/2NP1N2/PPP2PPP/R1BQKB1R w KQkq - 0 1");

    std::vector<int> moves = board.get_legal_moves();
    board.print_legal_moves(moves);

    board.print_board();
    return 0;
}