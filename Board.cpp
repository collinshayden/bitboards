//
// Created by Hayden Collins on 12/20/23.
//

#include "Board.h"
#include "MoveGeneration.h"

/// function to print board with unicode chars
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
    printf("\n    Enpass: %s", (enpassant_sq != no_sq) ? square_to_cord[enpassant_sq] : "no");
    printf("\n    Castling: %c%c%c%c\n\n", ((castling_rights & wk) ? 'K' : '-'), ((castling_rights & wq) ? 'Q' : '-'),
           ((castling_rights & bk) ? 'k' : '-'), ((castling_rights & bq) ? 'q' : '-'));
}

/// update board state from move
/// \param move
void Board::makeMove(int move) {
    // extract move data
    int source_square = get_move_source(move);
    int target_square = get_move_target(move);
    int piece = get_move_piece(move);
    int capture = get_move_capture(move);
    int promoted = get_move_promoted(move);
    int double_push = get_move_double_push(move);
    int en_passant = get_move_enpassant(move);
    int castling = get_move_castling(move);
    U64 source_target = (1ULL << source_square) | (1ULL << target_square); // bb of both source and target squares

    // save irreversible aspects of the position to history stacks
    en_passant_history.push(enpassant_sq);
    half_move_history.push(half_move);
    castling_rights_history.push(castling_rights);

    // if captured, remove it from the respective bitboard
    if (capture) {
        int captured_piece = get_move_captured_piece(move);
        // if the move is an en passant capture, remove the captured piece from correct square
        if (en_passant) {
            int ep_sq = side_to_move ? target_square + 8 : target_square - 8;
            pop_bit(piece_bitboards[!side_to_move], ep_sq);
        }
            // if not en passant, remove piece from expected target square
        else {
            pop_bit(piece_bitboards[captured_piece], target_square);
        }
    }
        // castling
    else if (castling) {
        if (target_square == g1) {
            // XOR masks
            // XOR piece bitboards with rooks on h1 and f1, XOR will delete rook on h1, and add rook on f1. Leaves other rooks alone
            // same idea for kings
            piece_bitboards[rook] ^= 0xa000000000000000;
            piece_bitboards[king] ^= 0x5000000000000000;
        } else if (target_square == c1) {
            piece_bitboards[rook] ^= 0x900000000000000;
            piece_bitboards[king] ^= 0x1400000000000000;
        } else if (target_square == g8) {
            piece_bitboards[rook + 1] ^= 0xa0;
            piece_bitboards[king + 1] ^= 0x50;
        } else if (target_square == c8) {
            piece_bitboards[rook + 1] ^= 0x9;
            piece_bitboards[king + 1] ^= 0x14;
        }
    }
        // if the move is a double push, set en passant square
    else if (double_push) {
        int ep_sq = side_to_move ? source_square + 8 : source_square - 8;
        enpassant_sq = ep_sq;
    } else {
        // if not double push, set enpassant_sq to no_sq
        enpassant_sq = no_sq;
    }

    // if promoted, put promoted piece in respective bitboard, else put piece on target square
    if (promoted) {
        set_bit(piece_bitboards[promoted], target_square);
    } else {
        set_bit(piece_bitboards[piece], target_square);
    }

    // if not capture or pawn push, increment half move counter
    if (!capture && piece != side_to_move) {
        half_move++;
    } else {
        half_move = 0;
    }

    // update castling rights
    // if white has kingside castling rights and if source square or target square is e1 or h1, white loses kingside castling rights
    if ((castling_rights & wk) && (0x9000000000000000 & source_target)) {
        castling_rights &= ~wk;
    }
    // if white has queenside castling rights and if source square or target square is e1 or a1, white loses queenside castling rights
    if ((castling_rights & wq) && (0x1100000000000000 & source_target)) {
        castling_rights &= ~wq;
    }
    // if black has kingside castling rights and if source square or target square is e8 or h8, black loses kingside castling rights
    if ((castling_rights & bk) && (0x90 & source_target)) {
        castling_rights &= ~bk;
    }
    // if black has queenside castling rights and if source square or target square is e8 or a8, black loses queenside castling rights
    if ((castling_rights & bq) && (0x11 & source_target)) {
        castling_rights &= ~bq;
    }

    // remove from piece bitboard
    pop_bit(piece_bitboards[piece], source_square);

    // update occupancies
    set_bit(occupancy_bitboards[side_to_move], target_square);
    set_bit(occupancy_bitboards[all], target_square);
    pop_bit(occupancy_bitboards[side_to_move], source_square);
    pop_bit(occupancy_bitboards[all], source_square);

    // if it is the end of black's turn, increment full move counter
    if (side_to_move) {
        full_move++;
    }

    side_to_move = !side_to_move;
}

/// undo move in board state
/// \param move
void Board::undoMove(int move) {
    int source_square = get_move_source(move);
    int target_square = get_move_target(move);
    int piece = get_move_piece(move);
    int capture = get_move_capture(move);
    int promoted = get_move_promoted(move);
    int en_passant = get_move_enpassant(move);
    int castling = get_move_castling(move);

    side_to_move = !side_to_move;

    // undo captures
    if (capture) {
        int captured_piece = get_move_captured_piece(move);
        // undo en passant
        if (en_passant) {
            int ep_sq = side_to_move ? target_square + 8 : target_square - 8;
            set_bit(piece_bitboards[!side_to_move], ep_sq);
        }
            // if not en passant, put piece back on expected target square
        else {
            set_bit(piece_bitboards[captured_piece], target_square);
        }
    }
        // undo castling
    else if (castling) {
        if (target_square == g1) {
            // XOR masks stay the same, and will undo the castling move
            piece_bitboards[rook] ^= 0xa000000000000000;
            piece_bitboards[king] ^= 0x5000000000000000;
        } else if (target_square == c1) {
            piece_bitboards[rook] ^= 0x900000000000000;
            piece_bitboards[king] ^= 0x1400000000000000;
        } else if (target_square == g8) {
            piece_bitboards[rook + 1] ^= 0xa0;
            piece_bitboards[king + 1] ^= 0x50;
        } else if (target_square == c8) {
            piece_bitboards[rook + 1] ^= 0x9;
            piece_bitboards[king + 1] ^= 0x14;
        }
    }

    // undo promotion
    if (promoted) {
        pop_bit(piece_bitboards[promoted], target_square);
    } else {
        pop_bit(piece_bitboards[piece], target_square);
    }

    // add back to piece bitboard
    set_bit(piece_bitboards[piece], source_square);

    // update occupancies
    pop_bit(occupancy_bitboards[side_to_move], target_square);
    pop_bit(occupancy_bitboards[all], target_square);
    set_bit(occupancy_bitboards[side_to_move], source_square);
    set_bit(occupancy_bitboards[all], source_square);

    // if it is the end of black's turn, decrement full move counter
    if (side_to_move) {
        full_move--;
    }

    // restore irreversible aspects of position from history stacks
    enpassant_sq = en_passant_history.top();
    en_passant_history.pop();

    half_move = half_move_history.top();
    half_move_history.pop();

    castling_rights = castling_rights_history.top();
    castling_rights_history.pop();

}

/// get legal moves of current position
/// \return vector of legal moves
std::vector<int> Board::get_legal_moves() {
    std::vector<int> legal_moves = generate_legal_moves(occupancy_bitboards, piece_bitboards, side_to_move,
                                                        castling_rights, enpassant_sq);
    return legal_moves;
}

/// print legal moves
/// \param legal_moves
void Board::print_legal_moves(const std::vector<int> &legal_moves) {
    for (int move: legal_moves) {
        if (get_move_promoted(move)) {
            printf("%s%s=%d capture: %d\n", square_to_cord[get_move_source(move)],
                   square_to_cord[get_move_target(move)], get_move_promoted(move), get_move_capture(move));
        } else {
            printf("%s%s capture: %d castling: %d en passant: %d\n", square_to_cord[get_move_source(move)],
                   square_to_cord[get_move_target(move)], get_move_capture(move), get_move_castling(move),
                   get_move_enpassant(move));
        }
    }
}


/// load board position from FEN position
/// \param FEN
void Board::load_FEN(const std::string &FEN) {
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
        enpassant_sq = no_sq;
    } else {
        int file = en_passant[0] - 'a';
        int rank = en_passant[1] - '0';
        enpassant_sq = (8 - rank) * 8 + file;
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

