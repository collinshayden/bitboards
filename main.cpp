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

// reference https://gist.github.com/peterellisjones/8c46c28141c162d1d8a0f0badbc9cff9
int tests() {
    fill_attack_tables();
    Board board;
    std::vector<int> moves;
    // test one (in check)
    board.load_FEN("r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 2\"");
    moves = board.get_legal_moves();
    assert(moves.size() == 8);

    // test two (en passant out of check)
    board.load_FEN("8/8/8/2k5/2pP4/8/B7/4K3 b - d3 0 3");
    moves = board.get_legal_moves();
    assert(moves.size() == 8);

    // test three (after 1.a3 Na6)
    board.load_FEN("r1bqkbnr/pppppppp/n7/8/8/P7/1PPPPPPP/RNBQKBNR w KQkq - 2 2");
    moves = board.get_legal_moves();
    assert(moves.size() == 19);

    // test four (in check)
    board.load_FEN("r3k2r/p1pp1pb1/bn2Qnp1/2qPN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQkq - 3 2");
    moves = board.get_legal_moves();
    assert(moves.size() == 5);

    printf("Passed all tests.\n");
    return 0;
}

int main() {
//    tests();
    fill_attack_tables();
    Board board;

    board.load_FEN("r3k2r/p1pp1pb1/bn2Qnp1/2qPN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQkq - 3 2");

    std::vector<int> moves = board.get_legal_moves();
    board.print_legal_moves(moves);

    board.print_board();

    printf("%d", b8 % 8);
    return 0;
}