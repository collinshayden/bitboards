//
// Created by Hayden Collins on 12/20/23.
//

#include "Board.h"
#include "MoveGeneration.h"

void Board::print_board() {
    printf("\n");
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            int piece = -1; // init as empty

            if (!file) {
                printf(" %d  ", 8 - rank);
            }
            for (int bb_piece = pawn; bb_piece <= king + 1; bb_piece++) {
                if (get_bit(Board::piece_bitboards[bb_piece], square)) {
                    piece = bb_piece;
                }
            }
            printf("%s ", (piece == -1) ? "." : unicode_pieces[piece]);
        }
        printf("\n");
    }
    printf("\n    a b c d e f g h\n");
    printf("\n    Side: %s", (side_to_move ? "black" : "white"));
    printf("\n    Enpass: %s", (enpassant != no_sq) ? square_to_cord[enpassant] : "no");
    printf("\n    Castling: %c%c%c%c\n\n", ((castling_rights & wk) ? 'K' : '-'), ((castling_rights & wq) ? 'Q' : '-'),
           ((castling_rights & bk) ? 'k' : '-'), ((castling_rights & bq) ? 'q' : '-'));
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
    else
        set_bit(piece_bitboards[piece], target_square);

    // remove from piece bitboard
    pop_bit(piece_bitboards[piece], source_square);

    // update occupancies
    set_bit(occupancy_bitboards[side_to_move], target_square);
    set_bit(occupancy_bitboards[all], target_square);
    pop_bit(occupancy_bitboards[side_to_move], source_square);
    pop_bit(occupancy_bitboards[all], source_square);
}

void Board::undoMove(int move) {
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
    else
        set_bit(piece_bitboards[piece], target_square);

    // remove from piece bitboard
    pop_bit(piece_bitboards[piece], source_square);

    // update occupancies
    set_bit(occupancy_bitboards[side_to_move], target_square);
    set_bit(occupancy_bitboards[all], target_square);
    pop_bit(occupancy_bitboards[side_to_move], source_square);
    pop_bit(occupancy_bitboards[all], source_square);
}

std::vector<int> Board::get_legal_moves() {
    std::vector<int> legal_moves = generate_legal_moves(occupancy_bitboards, piece_bitboards, side_to_move,
                                                        castling_rights);
    return legal_moves;
}

void Board::print_legal_moves(const std::vector<int> &legal_moves) {
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



/// load board position from FEN position
/// \param FEN
void Board::load_FEN(const std::string& FEN) {
    // init vars
    int pos = 0, square = 0, piece_type;

    // set all bitboards to empty
    std::fill(piece_bitboards, piece_bitboards + 12, 0ULL);
    std::fill(occupancy_bitboards, occupancy_bitboards + 3, 0ULL);

    std::istringstream iss(FEN);
    std::string board_state, side, castling, en_passant, half, full;

    getline(iss, board_state, ' ');
    getline(iss, side, ' ');
    getline(iss, castling, ' ');
    getline(iss, en_passant, ' ');
    getline(iss, half, ' ');
    getline(iss, full, ' ');

    // set side_to_move to move
    if (side[0] == 'w') side_to_move = 0;
    else side_to_move = 1;

    // set castling rights
    if (castling[0] == '-') castling_rights = 0;
    else {
        castling_rights = 0;
        for (char c: castling) {
            switch (c) {
                case 'K' :
                    castling_rights |= wk;
                    break;
                case 'Q' :
                    castling_rights |= wq;
                    break;
                case 'k' :
                    castling_rights |= bk;
                    break;
                case 'q' :
                    castling_rights |= bq;
                    break;
                default:
                    break;
            }
        }
    }

    // set en passant square
    if (en_passant[0] == '-') {
        enpassant = no_sq;
    } else {
        int file = en_passant[0] - 'a';
        int rank = en_passant[1] - '0';
        enpassant = (8 - rank) * 8 + file;
    }

    // set half and full move
    half_move = std::stoi(half);
    full_move = std::stoi(full);

    // set piece bitboards
    while (square < 64) {
        char c = board_state[pos];
        // if it is '/', do nothing and increment FEN pos
        if (c == '/') {
            pos++;
        }
            // if it is a number, that indicates blank squares on the rank
        else if (c >= '1' && c <= '8') {
            pos++;
            int num_empty = c - '0';
            square += num_empty;
        }

        // if it is a piece
        auto *result = std::find(std::begin(pieces), std::end(pieces), c);
        if (result != std::end(pieces)) {
            // get the piece type and set it on appropriate bitboard
            piece_type = std::distance(std::begin(pieces), result);
            set_bit(piece_bitboards[piece_type], square);

            // increment FEN position counter
            pos++;
            square++;
        }
    }

    // set occupancy
    for (int piece = 0; piece < 12; piece += 2) {
        occupancy_bitboards[white] |= piece_bitboards[piece];
        occupancy_bitboards[black] |= piece_bitboards[piece + 1];
    }
    occupancy_bitboards[all] = occupancy_bitboards[white] | occupancy_bitboards[black];
}

