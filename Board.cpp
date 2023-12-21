//
// Created by Hayden Collins on 12/20/23.
//

#include "Board.h"


void Board::print_board() {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            int piece = -1; // init as empty

            if (!file) {
                printf(" %d  ", 8 - rank);
            }
            for (int bb_piece = white_pawn; bb_piece <= black_king; bb_piece++) {
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
    printf("\n    Castling: %c%c%c%c\n\n", ((castle * wk) ? 'K' : '-'), ((castle * wq) ? 'Q' : '-'), ((castle * bk) ? 'k' : '-'), ((castle * bq) ? 'q' : '-'));
}

int main() {
//    Board board;
//    board.print_board();



    return 0;
}
