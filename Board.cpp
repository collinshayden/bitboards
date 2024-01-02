//
// Created by Hayden Collins on 12/20/23.
//

#include "Board.h"
#include "MoveGeneration.h"

void Board::print_board() {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            int piece = -1; // init as empty

            if (!file) {
                printf(" %d  ", 8 - rank);
            }
            for (int bb_piece = pawn; bb_piece <= king + 1; bb_piece++) {
                if(get_bit(Board::piece_bitboards[bb_piece], square)) {
                    piece = bb_piece;
                }
            }
            printf("%s ", (piece == -1) ? "." : unicode_pieces[piece]);
        }
        printf("\n");
    }
    printf("\n    a b c d e f g h\n");
    printf("\n    Side: %s", (side ? "white" : "black"));
    printf("\n    Enpass: %s", (enpassant != no_sq) ? square_to_cord[enpassant] : "no");
    printf("\n    Castling: %c%c%c%c\n\n", ((castling_rights * wk) ? 'K' : '-'), ((castling_rights * wq) ? 'Q' : '-'), ((castling_rights * bk) ? 'k' : '-'), ((castling_rights * bq) ? 'q' : '-'));
}

void Board::makeMove(int move) {
    int source_square = get_move_source(move);
    int target_square = get_move_target(move);
    int piece = get_move_piece(move);
    int capture = get_move_capture(move);
    int promoted = get_move_promoted(move);
    int double_push = get_move_double_push(move);
    int en_passant = get_move_enpassant(move);
    int castling = get_move_castling(move);

    // if captured, figure out what piece was captured and remove it from the respective bitboard
    if (capture) {
        for (int piece_type = 0; piece_type < 12; piece_type++) {
            if (get_bit(piece_bitboards[piece_type], target_square)) {
                pop_bit(piece_bitboards[piece_type], target_square);
            }
        }
    }
    // if promoted, put promoted piece in respective bitboard, else put piece on target square
    if (promoted) set_bit(piece_bitboards[promoted], target_square);
    else set_bit(piece_bitboards[piece], target_square);

    // remove from piece bitboard
    pop_bit(piece_bitboards[piece], source_square);

    // update occupancies
    set_bit(occupancy_bitboards[side], target_square);
    set_bit(occupancy_bitboards[all], target_square);
    pop_bit(occupancy_bitboards[side], source_square);
    pop_bit(occupancy_bitboards[all], source_square);
}

std::vector<int> Board::get_legal_moves() {
    std::vector<int> legal_moves = generate_legal_moves(occupancy_bitboards, piece_bitboards, side, castling_rights);
    return legal_moves;
}

void Board::print_legal_moves(const std::vector<int>& legal_moves) {
    for (int move: legal_moves) {
        if (get_move_promoted(move)) {
            printf("%s%s=%d capture: %d\n", square_to_cord[get_move_source(move)],
                   square_to_cord[get_move_target(move)], get_move_promoted(move), get_move_capture(move));
        } else {
            printf("%s%s capture: %d\n", square_to_cord[get_move_source(move)], square_to_cord[get_move_target(move)],
                   get_move_capture(move));
        }
    }

}




