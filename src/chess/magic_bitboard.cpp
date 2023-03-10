//
// Created by leon on 2019-08-05.
//

#include <vector>
#include <random>
#include <functional>
#include <chess/magic_bitboard.h>
#include <chess/bitboard.h>

namespace chess {
    namespace core {

        bitboard rook_table[102400];
        bitboard bishop_table[102400];
        magic rook_wizardry[64];
        magic bishop_wizardry[64];

        template<shift_direction d>
        bitboard calc_shift_attacks(const bitboard origin, const bitboard range, const bitboard occupancy) {
            auto sq = origin;
            bitboard attack = 0;
            while (range & sq) {
                sq = shift<d>(sq);
                attack |= sq;
                if (occupancy & sq) break;
            }
            return attack;
        }

        bitboard calc_rook_attacks(const bitboard origin, const bitboard occupancy) {
            return calc_shift_attacks<UP>(origin, rank_8_i, occupancy) |
                   calc_shift_attacks<DOWN>(origin, rank_1_i, occupancy) |
                   calc_shift_attacks<LEFT>(origin, file_a_i, occupancy) |
                   calc_shift_attacks<RIGHT>(origin, file_h_i, occupancy);
        }

        bitboard calc_bishop_attacks(const bitboard origin, const bitboard occupancy) {
            return calc_shift_attacks<UP_LEFT>(origin, file_a_i_rank_8_i, occupancy) |
                   calc_shift_attacks<DOWN_RIGHT>(origin, file_h_i_rank_1_i, occupancy) |
                   calc_shift_attacks<DOWN_LEFT>(origin, file_a_i_rank_1_i, occupancy) |
                   calc_shift_attacks<UP_RIGHT>(origin, file_h_i_rank_8_i, occupancy);
        }

        std::vector<bitboard> partitions(const bitboard attack_mask) {
            std::vector<bitboard> occupancy;

            bitboard bb = 0;
            do {
                occupancy.push_back(bb);
                bb -= attack_mask;
                bb &= attack_mask;
            } while (bb != 0);
            return occupancy;
        }

        void init_magic_bitboard(magic magics[], bitboard attack_table[],
                                 std::function<bitboard(bitboard, bitboard)> calc_attack,
                                 uint64_t precalc_magic_nums[]) {


            std::default_random_engine gen;
            std::uniform_int_distribution<uint64_t> dis(0, (uint64_t) -1);

            int last_size = 0;
            for (square sq = SQ_A1; sq <= SQ_H8; ++sq) {
                bitboard board_interior =
                        ((rank_8_i & rank_1_i) | rank[get_rank(sq)]) & ((file_a_i & file_h_i) | file[get_file(sq)]);
                //print_bb(board_interior);
                magic &m = magics[sq];

                bitboard origin = get_bb(sq);
                m.attack_mask = calc_attack(origin, 0) & board_interior;
                //print_bb(m.attack_mask);
                std::vector<bitboard> occupancies = partitions(m.attack_mask);
                std::vector<bitboard> attacks;
                attacks.reserve(occupancies.size());
                for (int i = 0; i < occupancies.size(); i++) {
                    attacks.push_back(calc_attack(origin, occupancies[i]));
                    //print_bb(attacks.back());
                }
                m.attack_table = &attack_table[last_size];
                m.shift = 64 - num_squares(m.attack_mask);
                last_size += occupancies.size();
                bool found = false;
                std::cout.flush();
                m.magic_number = precalc_magic_nums != nullptr ? precalc_magic_nums[sq] : 0;
                for (int attempt = 0; attempt < 1000000; attempt++) {
                    // fill the table
                    for (int i = 0; i < occupancies.size(); i++) {
                        m.attack_table[(occupancies[i] * m.magic_number) >> m.shift] = attacks[i];
                    }

                    // check if it works for all occupancies
                    found = true;
                    for (int i = 0; i < occupancies.size(); i++) {
                        if (m.attack_table[(occupancies[i] * m.magic_number) >> m.shift] != attacks[i]) {
                            found = false;
                            break;
                        }
                    }
                    if (found) break;

                    // get some random magic number
                    do {
                        auto r1 = dis(gen);
                        auto r2 = dis(gen);
                        auto r3 = dis(gen);
                        auto r4 = dis(gen);
                        m.magic_number = r1 & r2 & r3;
                    } while (!m.magic_number);
                }
                assert(found);
            }
        }

        void init_magic_bitboards() {
            std::default_random_engine gen;
            std::uniform_int_distribution<uint64_t> dis(0, (uint64_t) -1);

            // this is not necessary for the function to work, this is just the answer calculated without it
            // it's just pasted here to speed up the start-up, otherwise it will take a few seconds calculating it
            // through trial and error
            uint64_t pre_calculated_rook_numbers[] =
                    {0xa300108000210240, 0x4040100120084000, 0x2300184011002000, 0x0080080044801000, 0x0100040208001100,
                     0x0300040002410048, 0x0080020005000280, 0x120000a201028044, 0x400c800040128023, 0x40004001e0100842,
                     0x1001801000200484, 0x4000801002080080, 0x0480800800820400, 0xa0270008a3000400, 0x0802000802000421,
                     0x6001000082014100, 0x2300808000400020, 0x0432020025008348, 0x0004808010002001, 0x8440818010000800,
                     0x0a06220022021028, 0x7002808042000400, 0x20840c0001104208, 0xa1230200004c0081, 0x8400219080024000,
                     0x8001200240085000, 0x0201025100402000, 0x0008f00100210009, 0x0040850100080010, 0x048a001200150810,
                     0x4300918400081210, 0x1000110200008044, 0x0000804005800020, 0x1402010082002240, 0x4008200880801001,
                     0x1000210009001000, 0x0600040080800800, 0x0001000229000400, 0x0004410804001002, 0x008000884200012c,
                     0x8240008021428000, 0x484000281000a002, 0x0810002000808011, 0x01030010030b0020, 0x0200480004008080,
                     0x0041008400090002, 0x2800060001008080, 0x0010104403920009, 0x0801800041023100, 0x0c01002080400100,
                     0x400010822a004200, 0x0302080210008080, 0x0850080080040080, 0x0000040080020080, 0x0198021809500400,
                     0x1c0408840100c200, 0x0008408000102101, 0x0000220190804102, 0x0118428860001101, 0x8110100120042901,
                     0x008a000420d04802, 0x2801000a04004801, 0x0281001084220001, 0x103400c404810062};

            uint64_t pre_calculated_bishop_numbers[] =
                    {0x8050200800418028, 0x0090220a40420000, 0x043000c481040000, 0x03520a0200430008, 0x0c1410a840000800,
                     0x8001140e40001000, 0x00820b102804b000, 0x2002010403050805, 0x000021040c280840, 0x4100a00401020025,
                     0x2800240402820000, 0x001908084102d082, 0x0408040308404008, 0x4956008820080101, 0xe1000082080a4010,
                     0x8014044404049a01, 0x2440405006084500, 0xe02000032c010210, 0x0004002088048008, 0x0044040804204800,
                     0x0109001820080300, 0x0198404080504004, 0x0009480202062004, 0x8002420100423030, 0x01501060450c9001,
                     0x00080800601a208a, 0x6100300808008060, 0x4540040041820880, 0xa400840040802000, 0x0010c200a0411006,
                     0x0008010408540200, 0x10840024a5010150, 0x0108200440c80800, 0x4201301080460c05, 0x0040129010980040,
                     0x1840200800010106, 0x0000420020020081, 0x1042a80080011000, 0x802c040414015110, 0x9001010100052400,
                     0x0202080440041409, 0x080400ce10000800, 0x2010402404004040, 0x4080804208004084, 0x00010208a2046c00,
                     0x000310010a000140, 0x0030010204000080, 0x0008010042020285, 0x0014018404200802, 0x003a04c202104090,
                     0x0004022384108408, 0x904214aa84042000, 0x80000a100a022400, 0x2000041002420040, 0x0820204183010000,
                     0x4204010204330200, 0x0002004046082041, 0x8008202412084400, 0x02b0891200422204, 0x000a004390840c00,
                     0x4001400044904c00, 0x8004001021014d00, 0x0008088848080160, 0x00101000a1104604};

            init_magic_bitboard(rook_wizardry, rook_table, calc_rook_attacks, pre_calculated_rook_numbers);
            init_magic_bitboard(bishop_wizardry, bishop_table, calc_bishop_attacks, pre_calculated_bishop_numbers);
        }
    }
}