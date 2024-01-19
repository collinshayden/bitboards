//
// Created by Hayden Collins on 12/22/23.
//
#include "Board.h"
#include "MoveGeneration.h"

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

    // test five (rook pin with rook)
    board.load_FEN("k7/8/8/8/q2R2K1/8/8/8 w - - 0 1");
    moves = board.get_legal_moves();
    assert(moves.size() == 13);

    // test six (bishop pin with bishop)
    board.load_FEN("k7/8/5K2/8/3B4/8/1q6/8 w - - 0 1");
    moves = board.get_legal_moves();
    assert(moves.size() == 11);

    // test six (bishop pin with rook)
    board.load_FEN("k7/8/5K2/8/3R4/8/1q6/8 w - - 0 1");
    moves = board.get_legal_moves();
    assert(moves.size() == 8);

    // test seven (general position)
    board.load_FEN("2kr3r/p1ppqpb1/bn2Qnp1/3PN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQ - 3 2");
    moves = board.get_legal_moves();
    assert(moves.size() == 44);

    // test eight (promotion + other stuff)
    board.load_FEN("rnb2k1r/pp1Pbppp/2p5/q7/2B5/8/PPPQNnPP/RNB1K2R w KQ - 3 9");
    moves = board.get_legal_moves();
    assert(moves.size() == 39);

    // test nine (pinned pawn)
    board.load_FEN("2r5/3pk3/8/2P5/8/2K5/8/8 w - - 5 4");
    moves = board.get_legal_moves();
    assert(moves.size() == 9);

    // test ten (pinned pawn + illegal en passant)
    board.load_FEN("2r5/4k3/8/2Pp4/8/2K5/8/8 w - d6 5 4");
    moves = board.get_legal_moves();
    assert(moves.size() == 8);

    // test eleven (x ray illegal en passant)
    board.load_FEN("7k/8/8/q2Pp2K/8/8/8/8 w - e6 0 1");
    moves = board.get_legal_moves();
    assert(moves.size() == 6);

    printf("Passed all tests.\n");
    return 0;
}

U64 perft(int depth, int &nodes, Board &board) {
    if (depth == 0) {
        return 1ULL;
    }

    std::vector<int> legal_moves = board.get_legal_moves();
    nodes += legal_moves.size();
    for (int move : legal_moves) {

    }
}

int main() {
    tests();
    fill_attack_tables();
    Board board;

    board.load_FEN("r1bqkb1r/2pp1ppp/p1n2n2/1p2p3/4P3/1B3N2/PPPP1PPP/RNBQK2R w KQkq - 0 1");

    std::vector<int> moves = board.get_legal_moves();
    board.print_legal_moves(moves);
    printf("# legal moves: %zu\n", moves.size());

    board.print_board();

    board.makeMove(moves.at(2));

    board.print_board();

    board.undoMove(moves.at(2));

    board.print_board();

    return 0;
}