const int PAWN = 0;
const int KNIGHT = 1;
const int BISHOP = 2;
const int ROOK = 3;
const int QUEEN = 4;
const int KING = 5;
const int WHITE = 0;
const int BLACK = 6;
const int CHECKMASK = 12;
const int HV = 13;
const int D12 = 14;
const int ATTACK_MASK = 15;
const int GAME_STATE = 16;
const int OCCUPANCY_WHITE = 17;
const int OCCUPANCY_BLACK = 18;
const int OCCUPANCY = 19;
const int DEFENSE_MASK = 20;
const int BITBOARD_SIZE = 21;
/*
bits 0-3 - castling rights KQkq
bits 4-7 - en passant file (1-8, 0 - no)
bits 8-19 - half moves
bits 20-31 - full moves
bit 32 - move (0 - black)
*/
/*
Move
bits 0-11 - origin and destination
bits 12, 13 - special move (0 - normal, 1 - castle, 2 - en passant, 3 - promotion)
bits 14, 15 - piece (value + 1)
bits 16-18 - piece moved
*/
constexpr int INDEX64[64] = {
   63,  0, 58,  1, 59, 47, 53,  2,
   60, 39, 48, 27, 54, 33, 42,  3,
   61, 51, 37, 40, 49, 18, 28, 20,
   55, 30, 34, 11, 43, 14, 22,  4,
   62, 57, 46, 52, 38, 26, 32, 41,
   50, 36, 17, 19, 29, 10, 13, 21,
   56, 45, 25, 31, 35, 16,  9, 12,
   44, 24, 15,  8, 23,  7,  6,  5
};
unsigned long long KNIGHT_MASK[64];
unsigned long long KING_MASK[64];
unsigned long long BISHOP_MASK[64][512];
unsigned long long ROOK_MASK[64][4096];
unsigned long long BISHOP_RELEVANCY_MASK[64];
unsigned long long ROOK_RELEVANCY_MASK[64];
int ROOK_RELEVANT_BITS[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};
int BISHOP_RELEVANT_BITS[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};
unsigned long long BISHOP_MAGIC[64] = 
{4693318245697063424ULL, 
9009538955018240ULL, 
37159112425078912ULL, 
10377424047720892736ULL, 
307375210087055360ULL, 
2456856676586749952ULL, 
10376434846424645632ULL, 
18296425121580032ULL, 
72092787067592832ULL, 
175956253755456ULL, 
4982195634224ULL, 
36042270354506240ULL,
36033212782346240ULL, 
4612831743970804224ULL, 
2203587183744ULL, 
1155454925443178500ULL,
1171570390079111426ULL,
563087526740096ULL, 
4503603989481488ULL, 
18295942809944064ULL,
1125909044133892ULL, 
562954384770064ULL, 
70377367805952ULL, 
4785634090722304ULL, 
1200942114407560ULL, 
182398267534868736ULL, 
9223936086353642496ULL, 
81350735169716256ULL, 
10137497212354624ULL, 
2402432940261888ULL,
563568433172480ULL, 
282575595667648ULL, 
652496525443729440ULL, 
2308099224253908992ULL, 
162147762896371776ULL, 
105587476070660ULL, 
9224502351988002832ULL, 
1161929255764951104ULL, 
2306977707361173664ULL, 
2287538253340928ULL, 
580560468844544ULL, 
1130577193338880ULL, 
2305984022657927168ULL, 
275150602752ULL,
2305851809668792384ULL, 
4538818380169760ULL, 
4505253257052416ULL, 
283710674960640ULL,
36178364994699264ULL, 
282608881762304ULL,
612526968722161664ULL, 
2214854656ULL, 
68753164928ULL, 
2901448597641953280ULL, 
2286988552634368ULL, 
9293076606492672ULL,
19793424171264ULL, 
2200651171842ULL, 
142971425523712ULL,
18014952568914948ULL, 
36028803729981960ULL, 
72058015013732872ULL, 
1152956697603605540ULL, 
577621845239463952ULL};
unsigned long long ROOK_MAGIC[64] =
{9259403379333922944ULL, 
2323875000982970370ULL, 
3026437100125437985ULL, 
2666140875085582336ULL, 
144119621019107345ULL, 
144116579779608656ULL, 
72134560130990084ULL, 
180150032446533888ULL, 
5190539344544268320ULL, 
2322169095782467ULL, 
576742296536547396ULL, 
1774559025040461824ULL, 
72479840871123968ULL, 
9521313316891263488ULL,
2918614050726215936ULL,
3080602884759372032ULL, 
14411662293857206884ULL,
148624560168640512ULL, 
13511902692909056ULL, 
4613938368132153474ULL, 
141287378387968ULL, 
581527851707170818ULL, 
4398180827664ULL,
4611688217517920461ULL, 
1152991875500638210ULL,
7498493655523532800ULL,
2756484726101385216ULL, 
295548768496918592ULL,
2252075765596672ULL, 
4629981930568548354ULL, 
26405460148496ULL,
36310830341439748ULL, 
36029071901073408ULL, 
4625201352899837956ULL,
9236601430570180609ULL, 
4922435492781232136ULL, 
40673151322949632ULL, 
144678155242702864ULL, 
2199040098308ULL, 
1170936522833134593ULL,
6917669766203277352ULL,
1152956757702688769ULL, 
581254760477425680ULL,
2882374276291362824ULL, 
281492290863120ULL,
563018740137992ULL, 
2316257591941988356ULL, 
9281919383393927188ULL,
4614009287310573824ULL, 
1192364303483699456ULL,
720611126899900544ULL, 
140771849142400ULL, 
2459246940374697216ULL, 
14638899959955584ULL,
10556719010131755264ULL,
14411528154577437184ULL, 
71468801065011ULL, 
252348235090296854ULL,
4613383733637087297ULL, 
653309330971447297ULL,
72063400843214869ULL, 
11673611743816319491ULL, 
1441732432581099652ULL, 
1152952326368166150ULL};
const unsigned long long A_FILE = 0x101010101010101ULL;
const unsigned long long B_FILE = 0x202020202020202ULL;
const unsigned long long C_FILE = 0x404040404040404ULL;
const unsigned long long D_FILE = 0x808080808080808ULL;
const unsigned long long E_FILE = 0x1010101010101010ULL;
const unsigned long long F_FILE = 0x2020202020202020ULL;
const unsigned long long G_FILE = 0x4040404040404040ULL;
const unsigned long long H_FILE = 0x8080808080808080ULL;
const unsigned long long RANK_1 = 0xFFULL;
const unsigned long long RANK_2 = 0xFF00ULL;
const unsigned long long RANK_3 = 0xFF0000ULL;
const unsigned long long RANK_4 = 0xFF000000ULL;
const unsigned long long RANK_5 = 0xFF00000000ULL;
const unsigned long long RANK_6 = 0xFF0000000000ULL;
const unsigned long long RANK_7 = 0xFF000000000000ULL;
const unsigned long long RANK_8 = 0xFF00000000000000ULL;
const unsigned long long WHITE_HALF = RANK_1 | RANK_2 | RANK_3 | RANK_4;
const unsigned long long BLACK_HALF = ~WHITE_HALF;
constexpr unsigned long long RANKS[8] = {RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8};
constexpr unsigned long long FILES[8] = {A_FILE, B_FILE, C_FILE, D_FILE, E_FILE, F_FILE, G_FILE, H_FILE};
//const unsigned long long MAIN_DIAGONAL = 0x8040201008040201ULL;
//const unsigned long long SECONDARY_DIAGONAL = 0x102040810204080ULL;
const unsigned long long H8 = 0x8000000000000000ULL;
const unsigned long long H7G8 = 0x4080000000000000ULL;
const unsigned long long H6F8 = 0x2040800000000000ULL;
const unsigned long long H5E8 = 0x1020408000000000ULL;
const unsigned long long H4D8 = 0x810204080000000ULL;
const unsigned long long H3C8 = 0x408102040800000ULL;
const unsigned long long H2B8 = 0x204081020408000ULL;
const unsigned long long H1A8 = 0x102040810204080ULL;
const unsigned long long G1A7 = 0x1020408102040ULL;
const unsigned long long F1A6 = 0x10204081020ULL;
const unsigned long long E1A5 = 0x102040810ULL;
const unsigned long long D1A4 = 0x1020408ULL;
const unsigned long long C1A3 = 0x10204ULL;
const unsigned long long B1A2 = 0x102ULL;
const unsigned long long A1 = 0x1ULL;
constexpr const unsigned long long ANTI_DIAGONAL[15] = {A1, B1A2, C1A3, D1A4, E1A5, F1A6, G1A7, H1A8, H2B8, H3C8, H4D8, H5E8, H6F8, H7G8, H8};
const unsigned long long A8 = 0x100000000000000ULL;
const unsigned long long A7B8 = 0x201000000000000ULL;
const unsigned long long A6C8 = 0x402010000000000ULL;
const unsigned long long A5D8 = 0x804020100000000ULL;
const unsigned long long A4E8 = 0x1008040201000000ULL;
const unsigned long long A3F8 = 0x2010080402010000ULL;
const unsigned long long A2G8 = 0x4020100804020100ULL;
const unsigned long long A1H8 = 0x8040201008040201ULL;
const unsigned long long B1H7 = 0x80402010080402ULL;
const unsigned long long C1H6 = 0x804020100804ULL;
const unsigned long long D1H5 = 0x8040201008ULL;
const unsigned long long E1H4 = 0x80402010ULL;
const unsigned long long F1H3 = 0x804020ULL;
const unsigned long long G1H2 = 0x8040ULL;
const unsigned long long H1 = 0x80ULL;
constexpr unsigned long long DIAGONAL[15] = {H1, G1H2, F1H3, E1H4, D1H5, C1H6, B1H7, A1H8, A2G8, A3F8, A4E8, A5D8, A6C8, A7B8, A8};
const unsigned long long MAX = 0xFFFFFFFFFFFFFFFFULL;


