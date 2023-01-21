//
// Created by leon on 2019-07-29.
//

#ifndef CHESSENGINE_FEN_H
#define CHESSENGINE_FEN_H


#include <chess/board.h>

namespace chess::core::fen {
    struct fen_parts {
        std::string pieces;
        std::string side_to_move;
        std::string castling;
        std::string enpassant;
        std::string half_move_clock;
        std::string full_move_counter;
    };

    char fen_char(piece p, color c);
    std::string to_string(const board& b, int full_move_counter = 1);
    board board_from_fen(const std::string&);
    board board_from_fen_parts(const fen_parts& parts);
    fen_parts extract_fen_parts(const std::string& parts);
}


#endif //CHESSENGINE_FEN_H
