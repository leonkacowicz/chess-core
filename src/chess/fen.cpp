//
// Created by leon on 2019-07-29.
//

#include <chess/fen.h>

using namespace chess::core;

char fen::fen_char(piece p, color c) {
    const char offset = (char) (c == BLACK ? 'a' - 'A' : 0);
    switch (p) {
        case PAWN:
            return (char) ('P' + offset);
        case KNIGHT:
            return (char) ('N' + offset);
        case BISHOP:
            return (char) ('B' + offset);
        case ROOK:
            return (char) ('R' + offset);
        case QUEEN:
            return (char) ('Q' + offset);
        case KING:
            return (char) ('K' + offset);
        default:
            return '.';
    }
}

std::string fen::to_string(const board& b, int full_move_counter) {
    std::stringstream res;
    for (int r = 7; r >= 0; r--) {
        int rank_counter = 0;
        for (int f = 0; f < 8; f++) {
            auto sq = get_bb(f, r);
            piece p = b.piece_at(sq);
            if (p == NO_PIECE) {
                rank_counter++;
                continue;
            } else {
                if (rank_counter > 0) {
                    res << rank_counter;
                    rank_counter = 0;
                }
                color c = b.color_at(sq);
                res << fen_char(p, c);
            }
        }
        if (rank_counter > 0) res << rank_counter;
        if (r > 0) res << '/';
    }
    res << ' ' << (b.side_to_play == WHITE ? 'w' : 'b') << ' ';
    res << (b.can_castle_king_side[WHITE] ? 'K' : '-');
    res << (b.can_castle_queen_side[WHITE] ? 'Q' : '-');
    res << (b.can_castle_king_side[BLACK] ? 'k' : '-');
    res << (b.can_castle_queen_side[BLACK] ? 'q' : '-');

    res << ' ';
    if (b.en_passant == SQ_NONE) res << '-';
    else res << b.en_passant;
    res << ' ';

    res << (int) b.half_move_counter;
    res << ' ' << full_move_counter;
    return res.str();
}

board fen::board_from_fen(const std::string& fen) {
    return fen::board_from_fen_parts(fen::extract_fen_parts(fen));
}

board fen::board_from_fen_parts(const fen_parts& parts) {
    board b;
    using std::string;
    using std::stringstream;

    int r = 7;
    int f = 0;
    for (char ch : parts.pieces) {
        bitboard sq = file[f] & rank[r];
        if (ch == '/') {
            r--;
            f = 0;
            continue;
        }
        if (ch >= '1' && ch <= '7') {
            f += ch - '0';
            continue;
        }

        if (ch == 'p' || ch == 'P') b.piece_of_type[PAWN] |= sq;
        if (ch == 'n' || ch == 'N') b.piece_of_type[KNIGHT] |= sq;
        if (ch == 'b' || ch == 'B') b.piece_of_type[BISHOP] |= sq;
        if (ch == 'r' || ch == 'R') b.piece_of_type[ROOK] |= sq;
        if (ch == 'q' || ch == 'Q') b.piece_of_type[QUEEN] |= sq;
        if (ch == 'k') b.set_king_position(BLACK, get_square(sq));
        if (ch == 'K') b.set_king_position(WHITE, get_square(sq));
        if (ch == 'p' || ch == 'n' || ch == 'b' || ch == 'r' || ch == 'q' || ch == 'k') b.piece_of_color[BLACK] |= sq;
        if (ch == 'P' || ch == 'N' || ch == 'B' || ch == 'R' || ch == 'Q' || ch == 'K') b.piece_of_color[WHITE] |= sq;
        f++;
    }

    b.side_to_play = parts.side_to_move == "b" ? BLACK : WHITE;

    b.can_castle_king_side[WHITE] = parts.castling.find('K', 0) != std::string::npos;
    b.can_castle_queen_side[WHITE] = parts.castling.find('Q', 0) != std::string::npos;;
    b.can_castle_king_side[BLACK] = parts.castling.find('k', 0) != std::string::npos;;
    b.can_castle_queen_side[BLACK] = parts.castling.find('q', 0) != std::string::npos;;

    b.en_passant = parts.enpassant == "-" ? SQ_NONE : get_square(parts.enpassant.c_str());

    if (!parts.half_move_clock.empty())
        b.half_move_counter = (char)std::stoi(parts.half_move_clock);

    return b;
}

fen::fen_parts fen::extract_fen_parts(const std::string& fen) {
    std::istringstream ss(fen);
    fen_parts parts;
    getline(ss, parts.pieces, ' ');
    getline(ss, parts.side_to_move, ' ');
    getline(ss, parts.castling, ' ');
    getline(ss, parts.enpassant, ' ');
    getline(ss, parts.half_move_clock, ' ');
    getline(ss, parts.full_move_counter, ' ');

    return parts;
}
