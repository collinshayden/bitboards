//
// Created by Hayden Collins on 12/23/23.
//

#include "Tests.h"

void pawnsTestOne() {

    U64 white = 0ULL;
    set_bit(white, d4);

    U64 black = 0ULL;
    set_bit(black, e5);
    set_bit(black, c5);

    U64 piece_bitboards[12] = {white, black, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL,};
    // occupancies, white, black, both
    U64 occupancy_bitboards[3] = {white, black, white | black};

    int side = 0; // 0 white, 1 black

    std::vector<int> moves = pseudo_legal_moves(occupancy_bitboards, piece_bitboards, side);
    printf("\nWhite moves: %zu\n", moves.size());

    for (int move: moves) {
        printf("%s%s\n", square_to_cord[get_move_source(move)], square_to_cord[get_move_target(move)]);
    }

    assert(moves.size() == 3);

    side = 1; // 0 white, 1 black

    moves = pseudo_legal_moves(occupancy_bitboards, piece_bitboards, side);
    printf("\nBlack moves: %zu\n", moves.size());

    for (int move: moves) {
        printf("%s%s\n", square_to_cord[get_move_source(move)], square_to_cord[get_move_target(move)]);
    }

    assert(moves.size() == 4);
}

void knightsTestOne() {

    U64 white = 0ULL;
    set_bit(white, c6);

    U64 black = 0ULL;
    set_bit(black, b8);

    U64 piece_bitboards[12] = {0ULL, 0ULL, white, black, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL,};
    // occupancies, white, black, both
    U64 occupancy_bitboards[3] = {white, black, white | black};

    int side = 0; // 0 white, 1 black

    std::vector<int> moves = pseudo_legal_moves(occupancy_bitboards, piece_bitboards, side);
    printf("\nWhite moves: %zu\n", moves.size());

    for (int move: moves) {
        printf("%s%s\n", square_to_cord[get_move_source(move)], square_to_cord[get_move_target(move)]);
    }
    assert(moves.size() == 8);

    side = 1; // 0 white, 1 black

    moves = pseudo_legal_moves(occupancy_bitboards, piece_bitboards, side);
    printf("\nBlack moves: %zu\n", moves.size());

    for (int move: moves) {
        printf("%s%s\n", square_to_cord[get_move_source(move)], square_to_cord[get_move_target(move)]);
    }

    assert(moves.size() == 3);
}

void bishopsTestOne() {
    U64 white = 0ULL;
    set_bit(white, c6);

    U64 black = 0ULL;
    set_bit(black, b8);

    U64 piece_bitboards[12] = {0ULL, 0ULL, 0ULL, 0ULL, white, black, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL,};
    // occupancies, white, black, both
    U64 occupancy_bitboards[3] = {white, black, white | black};

    int side = 0; // 0 white, 1 black

    std::vector<int> moves = pseudo_legal_moves(occupancy_bitboards, piece_bitboards, side);
    printf("\nWhite moves: %zu\n", moves.size());

    for (int move: moves) {
        printf("%s%s\n", square_to_cord[get_move_source(move)], square_to_cord[get_move_target(move)]);
    }
    assert(moves.size() == 11);

    side = 1; // 0 white, 1 black

    moves = pseudo_legal_moves(occupancy_bitboards, piece_bitboards, side);
    printf("\nBlack moves: %zu\n", moves.size());

    for (int move: moves) {
        printf("%s%s\n", square_to_cord[get_move_source(move)], square_to_cord[get_move_target(move)]);
    }

    assert(moves.size() == 7);
}

void rooksTestOne() {
    U64 white = 0ULL;
    set_bit(white, c6);

    U64 black = 0ULL;
    set_bit(black, a8);

    U64 piece_bitboards[12] = {0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, white, black, 0ULL, 0ULL, 0ULL, 0ULL,};
    // occupancies, white, black, both
    U64 occupancy_bitboards[3] = {white, black, white | black};

    int side = 0; // 0 white, 1 black

    std::vector<int> moves = pseudo_legal_moves(occupancy_bitboards, piece_bitboards, side);
    printf("\nWhite moves: %zu\n", moves.size());

    for (int move: moves) {
        printf("%s%s\n", square_to_cord[get_move_source(move)], square_to_cord[get_move_target(move)]);
    }
    assert(moves.size() == 14);

    side = 1; // 0 white, 1 black

    moves = pseudo_legal_moves(occupancy_bitboards, piece_bitboards, side);
    printf("\nBlack moves: %zu\n", moves.size());

    for (int move: moves) {
        printf("%s%s\n", square_to_cord[get_move_source(move)], square_to_cord[get_move_target(move)]);
    }

    assert(moves.size() == 14);
}

void queensTestOne() {
    U64 white = 0ULL;
    set_bit(white, d4);
    print_bitboard(white);


    U64 piece_bitboards[12] = {0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, white, 0ULL, 0ULL, 0ULL,};
    // occupancies, white, black, both
    U64 occupancy_bitboards[3] = {white, 0ULL, white};

    int side = 0; // 0 white, 1 black

    std::vector<int> moves = pseudo_legal_moves(occupancy_bitboards, piece_bitboards, side);
    printf("\nWhite moves: %zu\n", moves.size());

    for (int move: moves) {
        printf("%s%s\n", square_to_cord[get_move_source(move)], square_to_cord[get_move_target(move)]);
    }
    assert(moves.size() == 27);
}

void kingsTestOne() {
    U64 white = 0ULL;
    set_bit(white, d4);
    print_bitboard(white);


    U64 piece_bitboards[12] = {0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, white, 0ULL,};
    // occupancies, white, black, both
    U64 occupancy_bitboards[3] = {white, 0ULL, white};

    int side = 0; // 0 white, 1 black

    std::vector<int> moves = pseudo_legal_moves(occupancy_bitboards, piece_bitboards, side);
    printf("\nWhite moves: %zu\n", moves.size());

    for (int move: moves) {
        printf("%s%s\n", square_to_cord[get_move_source(move)], square_to_cord[get_move_target(move)]);
    }
    assert(moves.size() == 8);
}

int main() {
    fill_attack_tables();

    pawnsTestOne();
    knightsTestOne();
    bishopsTestOne();
    rooksTestOne();
    queensTestOne();
    kingsTestOne();
}
