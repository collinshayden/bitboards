//
// Created by Hayden Collins on 12/22/23.
//
#include "Board.h"
#include "MoveGeneration.h"
#include "iostream"

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

U64 perft(int depth, Board &board, int &captures, int &ep, int &castles, int &promotions, int &num) {
    if (depth == 0) {
        return 1ULL;
    }
    U64 nodes = 0;

    std::vector<int> legal_moves = board.get_legal_moves();
    for (int move: legal_moves) {
        if (get_move_capture(move)) {
            captures++;
            board.print_board();
            printf("%d. dep=%d %s to %s\n", num, depth, square_to_cord[get_move_source(move)], square_to_cord[get_move_target(move)]);
            num++;
        }
        if (get_move_enpassant(move)) ep++;
        if (get_move_castling(move)) castles++;
        if (get_move_promoted(move)) {
            promotions++;

        }
        board.makeMove(move);
        nodes += perft(depth - 1, board, captures, ep, castles, promotions, num);
        board.undoMove(move);
    }
    return nodes;
}

void run_perft(int max_depth, std::string fen, Board &board) {
    board.load_FEN(fen);
    for (int depth = 1; depth <= max_depth; depth++) {
//        board.load_FEN(fen);
        int captures = 0 , ep = 0, castles = 0, promotions = 0, num = 1;
        U64 nodes = perft(depth, board, captures, ep, castles, promotions, num);
        printf("nodes at depth %d = %llu, captures = %d, ep = %d, castles = %d, promotions = %d\n", depth, nodes, captures, ep, castles, promotions);
    }
}

int main() {
//    tests();
    fill_attack_tables();
    Board board;

//    board.load_FEN("rnbqkbnr/1ppppppp/p7/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1");
//    board.load_FEN("1k6/8/8/4Pp2/8/8/1K6/8 w - f6 0 1");
//
//
//    std::vector<int> moves = board.get_legal_moves();
//    board.print_legal_moves(moves);
//    int move = moves.at(14);
//    board.print_board();
//    board.makeMove(move);
//    board.print_board();
//
//    moves = board.get_legal_moves();
//    board.print_legal_moves(moves);
//
//    move = moves.at(3);
//    board.print_board();
//    board.makeMove(move);
//    board.print_board();
//
//    moves = board.get_legal_moves();
//    board.print_legal_moves(moves);
//
//    move = moves.at(7);
//    board.print_board();
//    board.makeMove(move);
//    board.print_board();
//
//    moves = board.get_legal_moves();
//    board.print_legal_moves(moves);

//    printf("%d\n", get_move_promoted(move));
//
//
//
//    board.undoMove(move);
//    board.print_board();
//
    run_perft(4, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", board);
    // positions 991 and 17 seem to be identical
    // position 1280, 1292 seems illegal (bishop on c8 capturing h3 ignoring pawn on g4)
    return 0;
}