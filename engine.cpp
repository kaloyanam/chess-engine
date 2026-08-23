#include "utils.h"
//#define DEBUG
using namespace std;
unsigned long long bitboard[BITBOARD_SIZE];
int piece_board[64];

inline unsigned long long findMagicBishop(int pos) {
    int variations = 1 << BISHOP_RELEVANT_BITS[pos];
    unsigned long long occupancies[variations];
    for(int i = 0; i < variations; i++) {
        occupancies[i] = 0;
        for(unsigned long long j = 0, p = i, q = BISHOP_RELEVANCY_MASK[pos]; p != 0; j++) {
            if((p & 0x1ULL) == 1) {
                occupancies[i] += q&-q; 
            }
            q -= q&-q;
            p/=2;
        }
    }
    bool used[variations];
    while(true) {
        bool fail = false;
        unsigned long long candidate = random64() & random64() & random64() & random64();
         if(__builtin_popcountll((BISHOP_RELEVANCY_MASK[pos] * candidate) & 0xFF00000000000000ULL) < 6) continue;
        //cout << candidate << endl;
        memset(used, false, sizeof(bool) * variations);
        for(int i = 0; i < variations; i++) {
            int idx = (occupancies[i] * candidate) >> (64 - BISHOP_RELEVANT_BITS[pos]);
            if(used[idx]) {
                fail = true;
                break;
            }
            used[idx] = true;
        }
        if(!fail) return candidate;
    }
}
inline unsigned long long findMagicRook(int pos) {
    int variations = 1 << ROOK_RELEVANT_BITS[pos];
    unsigned long long occupancies[variations];
    for(int i = 0; i < variations; i++) {
        occupancies[i] = 0;
        for(unsigned long long j = 0, p = i, q = ROOK_RELEVANCY_MASK[pos]; p != 0; j++) {
            if((p & 0x1ULL) == 1) {
                occupancies[i] += q&-q; 
            }
            q -= q&-q;
            p/=2;
        }
        //cout << bin(occupancies[i]) << "\n\n";
    }
    bool used[variations];
    int counter = 0;
    while(true) {
        bool fail = false;
        unsigned long long candidate = random64() & random64() & random64();
        if(__builtin_popcount((ROOK_RELEVANCY_MASK[pos] * candidate) & 0xFF00000000000000ULL) < 6) continue;
        //cout << candidate << endl;
        memset(used, false, sizeof(bool) * variations);
        for(int i = 0; i < variations; i++) {
            int idx = (occupancies[i] * candidate) >> (64 - ROOK_RELEVANT_BITS[pos]);
            if(used[idx]) {
                fail = true;
                //cout << ++counter << endl;
                break;
            }
            used[idx] = true;
        }
        if(!fail) return candidate;
    }
}

inline void generateBitstrings() {
    //cout << "= {";
    for(int i = 0; i < 64; i++) {
        //if(i != 0) cout << ", ";
        //cout << "{";
        for(int j = 0; j < 12; j++) {
            //if(j != 0) cout << ", ";
            ZOBRIST_BITSTRINGS[i][j] = random64();
            //cout << ZOBRIST_BITSTRINGS[i][j] << "ULL";
        }
        //cout << "}";
    }
    //cout << "};";
    ZOBRIST_BLACK_TO_MOVE = random64();
    //cout << ZOBRIST_BLACK_TO_MOVE << "ULL";
    cout << "Generated bitstrings\n";
}

inline unsigned long long zobrist(unsigned long long board[]) {
    unsigned long long hash = 0;
    if(getSide(board) == BLACK) hash ^= ZOBRIST_BLACK_TO_MOVE;
    for(int i = 0; i < 12; i++) {
        unsigned long long curr = board[i];
        for(unsigned long long j = board[i]; j != 0; j -= j&-j) {
            int pos = bitscan(j&-j);
            hash ^= ZOBRIST_BITSTRINGS[pos][i];
        }
    }
    return hash;
}

inline void precomputeMasks() {
    srand(time(NULL));
    //generateBitstrings();
    //knight
    for(int i = 0; i < 64; i++) {
        //knight mask
        KNIGHT_MASK[i] = 0;
        KNIGHT_MASK[i] += (i - 17 >= 0) ? (1ULL << (i - 17) & ~H_FILE) : 0;
        KNIGHT_MASK[i] += (i - 15 >= 0) ? (1ULL << (i - 15) & ~A_FILE) : 0;
        KNIGHT_MASK[i] += (i - 10 >= 0) ? (1ULL << (i - 10) & ~(G_FILE | H_FILE)) : 0;
        KNIGHT_MASK[i] += (i - 6 >= 0) ? (1ULL << (i - 6) & ~(B_FILE | A_FILE)) : 0;
        KNIGHT_MASK[i] += (i + 17 < 64) ? (1ULL << (i + 17) & ~A_FILE) : 0;
        KNIGHT_MASK[i] += (i + 15 < 64) ? (1ULL << (i + 15) & ~H_FILE) : 0;
        KNIGHT_MASK[i] += (i + 10 < 64) ? (1ULL << (i + 10) & ~(B_FILE | A_FILE)) : 0;
        KNIGHT_MASK[i] += (i + 6 < 64) ? (1ULL << (i + 6) & ~(G_FILE | H_FILE)) : 0;

        //king mask
        KING_MASK[i] = 0;
        KING_MASK[i] += (i - 9 >= 0) ? (1ULL << (i - 9) & ~H_FILE) : 0;
        KING_MASK[i] += (i - 8 >= 0) ? 1ULL << (i - 8) : 0;
        KING_MASK[i] += (i - 7 >= 0) ? (1ULL << (i - 7) & ~A_FILE) : 0;
        KING_MASK[i] += (i - 1 >= 0) ? (1ULL << (i - 1) & ~H_FILE) : 0;
        KING_MASK[i] += (i + 1 < 64) ? (1ULL << (i + 1) & ~A_FILE) : 0;
        KING_MASK[i] += (i + 7 < 64) ? (1ULL << (i + 7) & ~H_FILE) : 0;
        KING_MASK[i] += (i + 8 < 64) ? 1ULL << (i + 8) : 0;
        KING_MASK[i] += (i + 9 < 64) ? (1ULL << (i + 9) & ~A_FILE) : 0;

        //bishop relevancy mask
        //down left
        for(int j = i - 9; j >= 0 && (j & 0x7ULL) != 7; j -= 9) {
            BISHOP_RELEVANCY_MASK[i] += (1ULL << j);
        }
        //down right
        for(int j = i - 7; j >= 0 && (j & 0x7ULL) != 0; j -= 7) {
            BISHOP_RELEVANCY_MASK[i] += (1ULL << j);
        }
        //up left
        for(int j = i + 7; j < 64 && (j & 0x7ULL) != 7; j += 7) {
            BISHOP_RELEVANCY_MASK[i] += (1ULL << j);
        }
        //up right
        for(int j = i + 9; j < 64 && (j & 0x7ULL) != 0; j += 9) {
            BISHOP_RELEVANCY_MASK[i] += (1ULL << j);
        }
        BISHOP_RELEVANCY_MASK[i] &= ~(RANK_1 | RANK_8 | A_FILE | H_FILE);
        /*BISHOP_MAGIC[i] = findMagicBishop(i);
        cout << BISHOP_MAGIC[i] << "ULL, " << endl;*/
        int variations = 1 << BISHOP_RELEVANT_BITS[i];
        for(int k = 0; k < variations; k++) {
            unsigned long long current = 0;
            for(unsigned long long j = 0, p = k, q = BISHOP_RELEVANCY_MASK[i]; p != 0; j++) {
                if((p & 0x1ULL) == 1) {
                    current += q&-q; 
                }
                q -= q&-q;
                p/=2;
            }
            int idx = (current * BISHOP_MAGIC[i]) >> (64 - BISHOP_RELEVANT_BITS[i]);
            BISHOP_MASK[i][idx] = 0;
            //down left
            for(int j = i - 9; j >= 0 && (j & 0x7ULL) != 7; j -= 9) {
                BISHOP_MASK[i][idx] += (1ULL << j);
                if(((current >> j) & 0x1ULL) == 1) break;
            }
            //down right
            for(int j = i - 7; j >= 0 && (j & 0x7ULL) != 0; j -= 7) {
                BISHOP_MASK[i][idx] += (1ULL << j);
                if(((current >> j) & 0x1ULL) == 1) break;
            }
            //up left
            for(int j = i + 7; j < 64 && (j & 0x7ULL) != 7; j += 7) {
                BISHOP_MASK[i][idx] += (1ULL << j);
                if(((current >> j) & 0x1ULL) == 1) break;
            }
            //up right
            for(int j = i + 9; j < 64 && (j & 0x7ULL) != 0; j += 9) {
                BISHOP_MASK[i][idx] += (1ULL << j);
                if(((current >> j) & 0x1ULL) == 1) break;
            }
        }
        //rook relevancy mask
        //down
        for(int j = i - 8; j >= 8; j -= 8) {
            ROOK_RELEVANCY_MASK[i] += (1ULL << j);
        }
        //left
        for(int j = i - 1; j >= 0 && (i & 0x7ULL) != 0 && (j & 0x7ULL) != 0; j--) {
            ROOK_RELEVANCY_MASK[i] += (1ULL << j);
        }
        //up
        for(int j = i + 8; j < 56; j += 8) {
            ROOK_RELEVANCY_MASK[i] += (1ULL << j);
        }
        //right
        for(int j = i + 1; (i & 0x7ULL) != 7 && j < 64 && (j & 0x7ULL) != 7; j++) {
            ROOK_RELEVANCY_MASK[i] += (1ULL << j);
        }
        /*ROOK_MAGIC[i] = findMagicRook(i);
        cout << ROOK_MAGIC[i] << "ULL, " << endl;*/
        variations = 1 << ROOK_RELEVANT_BITS[i];
        for(int k = 0; k < variations; k++) {
            unsigned long long current = 0;
            for(unsigned long long j = 0, p = k, q = ROOK_RELEVANCY_MASK[i]; p != 0; j++) {
                if((p & 0x1ULL) == 1) {
                    current += q&-q; 
                }
                q -= q&-q;
                p/=2;
            }
            int idx = (current * ROOK_MAGIC[i]) >> (64 - ROOK_RELEVANT_BITS[i]);
            ROOK_MASK[i][idx] = 0;
            //down
            for(int j = i - 8; j >= 0; j -= 8) {
                ROOK_MASK[i][idx] += (1ULL << j);
                if(((current >> j) & 0x1ULL) == 1) break;
            }
            //left
            for(int j = i - 1; j >= 0 && (j & 0x7ULL) != 7; j--) {
                ROOK_MASK[i][idx] += (1ULL << j);
                if(((current >> j) & 0x1ULL) == 1) break;
            }
            //up
            for(int j = i + 8; j < 64; j += 8) {
                ROOK_MASK[i][idx] += (1ULL << j);
                if(((current >> j) & 0x1ULL) == 1) break;
            }
            //right
            for(int j = i + 1; j < 64 &&(j & 0x7ULL) != 0; j++) {
                ROOK_MASK[i][idx] += (1ULL << j);
                if(((current >> j) & 0x1ULL) == 1) break;
            }
        }
    }
}

inline unsigned long long occupiedMask(int side, unsigned long long board[]) {
    unsigned long long result = 0;
    if(side == -1) {
        for(int i = 0; i < 12; i++) {
            result |= board[i];
        }
    }
    else if(side == WHITE) {
        for(int i = 0; i < 6; i++) {
            result |= board[i];
        }
    }
    else {
        for(int i = 6; i < 12; i++) {
            result |= board[i];
        }
    }

    return result;
}

inline void drawBoard(unsigned long long board[]) {
    for(int i = 7; i >= 0; i--) {
        for(int j = 0; j < 8; j++) {
            //white pawn
            if((board[WHITE + PAWN] >> ((i * 8 + j)) & 0x1ULL) == 1) {
                cout << 'P';
            }
            //black pawn
            else if((board[BLACK + PAWN] >> ((i * 8 + j)) & 0x1ULL) == 1) {
                cout << 'p';
            }
            //white knight
            else if((board[WHITE + KNIGHT] >> ((i * 8 + j)) & 0x1ULL) == 1) {
                cout << 'N';
            }
            //black knight
            else if((board[BLACK + KNIGHT] >> ((i * 8 + j)) & 0x1ULL) == 1) {
                cout << 'n';
            }
            //white bishop
            else if((board[WHITE + BISHOP] >> ((i * 8 + j)) & 0x1ULL) == 1) {
                cout << 'B';
            }
            //black bishop
            else if((board[BLACK + BISHOP] >> ((i * 8 + j)) & 0x1ULL) == 1) {
                cout << 'b';
            }
            //white rook
            else if((board[WHITE + ROOK] >> ((i * 8 + j)) & 0x1ULL) == 1) {
                cout << 'R';
            }
            //black rook
            else if((board[BLACK + ROOK] >> ((i * 8 + j)) & 0x1ULL) == 1) {
                cout << 'r';
            }
            //white queen
            else if((board[WHITE + QUEEN] >> ((i * 8 + j)) & 0x1ULL) == 1) {
                cout << 'Q';
            }
            //black queen
            else if((board[BLACK + QUEEN] >> ((i * 8 + j)) & 0x1ULL) == 1) {
                cout << 'q';
            }
            //white king
            else if((board[WHITE + KING] >> ((i * 8 + j)) & 0x1ULL) == 1) {
                cout << 'K';
            }
            //black king
            else if((board[BLACK + KING] >> ((i * 8 + j)) & 0x1ULL) == 1) {
                cout << 'k';
            }
            //empty
            else {
                cout << '.';
            }
            cout << " ";
        }
        cout << "\n";
    }
    cout << "\nCastling rights: " << ((board[GAME_STATE] & 0x1ULL) != 0 ? "K" : "") << ((board[GAME_STATE] & 0x2ULL) != 0 ? "Q" : "") << ((board[GAME_STATE] & 0x4ULL) != 0 ? "k" : "") << ((board[GAME_STATE] & 0x8ULL) != 0 ? "q" : "");
    cout << "\nEn Passant: " << (((board[GAME_STATE] >> 4) & 0xFULL) ? string() + (char)('a' - 1 + (((board[GAME_STATE] >> 4) & 0xFULL))) : "-");
    cout << "\nTurn: " << (((bitboard[GAME_STATE] >> 32) & 0x1ULL) ? "Black" : "White");
    cout << "\n\n";
}

inline constexpr unsigned long long getHVRay(unsigned long long king, unsigned long long rook) {
    int position = bitscan(rook);
    int kingPosition = bitscan(king);
    if(position / 8 == kingPosition / 8) {
        if(rook < king) {
            return king - rook;
        }
        else {
            return (rook - king) << 1;
        }
    }
    else {
        if(rook < king) {
            
            return (king - rook) & FILES[position & 0x7ULL];
        }
        else {
            return ((rook - king) << 1) & FILES[position & 0x7ULL];
        }
    }
}

inline constexpr unsigned long long getD12Ray(unsigned long long king, unsigned long long bishop) {
    int position = bitscan(bishop);
    int kingPosition = bitscan(king);
    int file = position & 0x7ULL;
    int rank = position / 8;
    if(rank - file == kingPosition / 8 - (kingPosition & 0x7ULL)) {
        if(bishop < king) {
            return (king - bishop) & DIAGONAL[7 + rank - file];
        }
        else {
            return ((bishop - king) << 1) & DIAGONAL[7 + rank - file];
        } 
    } 
    else {
        if(bishop < king) {
            return (king - bishop) & ANTI_DIAGONAL[rank + file];
        }
        else {
            return ((bishop - king) << 1) & ANTI_DIAGONAL[rank + file];
        } 
    }
}

inline void pawnCompute(int side, unsigned long long board[]) {
    unsigned long long kingPosition = board[side + KING];
    side = BLACK - side;
    //right
    unsigned long long took = (side == WHITE) ? (board[side + PAWN] << 9) : (board[side + PAWN] >> 7);
    took &= ~A_FILE;
    if(took & kingPosition) {
        if(board[CHECKMASK] == MAX) {
            board[CHECKMASK] = (side == WHITE ? (took & kingPosition) >> 9 : (took & kingPosition) << 7);
            int enPassantFile = ((board[GAME_STATE] >> 4) & 0xFULL);
            if(enPassantFile && (bitscan((side == WHITE ? (took & kingPosition) >> 9 : (took & kingPosition) << 7)) & 0x7ULL) == enPassantFile - 1) {
                board[CHECKMASK] += (1ULL << (side == BLACK ? 40 : 16) + enPassantFile - 1);
            }
        } else {
            board[CHECKMASK] = 0;
        }
    }
    board[ATTACK_MASK] |= took;
    //left
    took = (side == WHITE) ? (board[side + PAWN] << 7) : (board[side + PAWN] >> 9);
    took &= ~H_FILE;
    if(took & kingPosition) {
        if(board[CHECKMASK] == MAX) {
            board[CHECKMASK] = side == WHITE ? (took & kingPosition) >> 7 : (took & kingPosition) << 9;
            int enPassantFile = (board[GAME_STATE] >> 4) & 0xFULL;
            if(enPassantFile && (bitscan(side == WHITE ? (took & kingPosition) >> 7 : (took & kingPosition) << 9) & 0x7ULL) == enPassantFile - 1) {
                board[CHECKMASK] += (1ULL << (side == BLACK ? 40 : 16) + enPassantFile - 1);
            }
        } else {
            board[CHECKMASK] = 0;
        }
    }
    board[ATTACK_MASK] |= took;
}

inline void knightCompute(int side, unsigned long long board[]) {
    unsigned long long kingPosition = board[side + KING];
    side = BLACK - side;
    for(unsigned long long i = board[side + KNIGHT]; i != 0; i -= i&-i) {
        unsigned long long moveMap = KNIGHT_MASK[bitscan(i&-i)];
        if(moveMap & kingPosition) {
            if(board[CHECKMASK] == MAX) {
                board[CHECKMASK] = i&-i;
            } else {
                board[CHECKMASK] = 0;
            }
        }
        board[ATTACK_MASK] |= moveMap;
    }
}

inline void rookCompute(int side, unsigned long long board[]) {
    unsigned long long kingPosition = board[side + KING];
    side = BLACK - side;
    unsigned long long occupied = board[OCCUPANCY] - kingPosition;
    for(unsigned long long i = board[side + ROOK]; i != 0; i -= i&-i) {
        int position = bitscan(i&-i);
        unsigned long long moveMap = ROOK_MASK[position][(ROOK_RELEVANCY_MASK[position] & occupied) * ROOK_MAGIC[position] >> (64 - ROOK_RELEVANT_BITS[position])];
        if(moveMap & kingPosition) {
            if(board[CHECKMASK] == MAX) {
                board[CHECKMASK] = getHVRay(kingPosition, i&-i);
            } else {
                board[CHECKMASK] = 0;
            }
        }
        if(position / 8 == bitscan(kingPosition) / 8 || (position & 0x7ULL) == (bitscan(kingPosition) & 0x7ULL)) {
            unsigned long long ray = getHVRay(kingPosition, i&-i);
            if(__builtin_popcountll((ray - (i&-i)) & occupied) == 1) {
                board[HV] |= ray;
            }
        }
        board[ATTACK_MASK] |= moveMap;
    }
}

inline void bishopCompute(int side, unsigned long long board[]) {
    unsigned long long kingPosition = board[side + KING];
    side = BLACK - side;
    unsigned long long occupied = board[OCCUPANCY] - kingPosition;
    for(unsigned long long i = board[side + BISHOP]; i != 0; i -= i&-i) {
        int position = bitscan(i&-i);
        unsigned long long moveMap = BISHOP_MASK[position][(BISHOP_RELEVANCY_MASK[position] & occupied) * BISHOP_MAGIC[position] >> (64 - BISHOP_RELEVANT_BITS[position])];
        if(moveMap & kingPosition) {
            if(board[CHECKMASK] == MAX) {
                board[CHECKMASK] = getD12Ray(kingPosition, i&-i);
            } else {
                board[CHECKMASK] = 0;
            }
        }
        if(position / 8 - (position & 0x7ULL) == bitscan(kingPosition) / 8 - (bitscan(kingPosition) & 0x7ULL) || position / 8 + (position & 0x7ULL) == bitscan(kingPosition) / 8 + (bitscan(kingPosition) & 0x7ULL)) {
            unsigned long long ray = getD12Ray(kingPosition, i&-i);
            if(__builtin_popcountll((ray - (i&-i)) & occupied) == 1) {
                board[D12] |= ray;
            }
        }
        board[ATTACK_MASK] |= moveMap;
    }
}

inline void queenCompute(int side, unsigned long long board[]) {
    unsigned long long kingPosition = board[side + KING];
    side = BLACK - side;
    unsigned long long occupied = board[OCCUPANCY] - kingPosition;
    for(unsigned long long i = board[side + QUEEN]; i != 0; i -= i&-i) {
        int position = bitscan(i&-i);
        unsigned long long moveMap = BISHOP_MASK[position][(BISHOP_RELEVANCY_MASK[position] & occupied) * BISHOP_MAGIC[position] >> (64 - BISHOP_RELEVANT_BITS[position])];
        if(moveMap & kingPosition) {
            if(board[CHECKMASK] == MAX) {
                board[CHECKMASK] = getD12Ray(kingPosition, i&-i);
            } else {
                board[CHECKMASK] = 0;
            }
        }
        if(position / 8 - (position & 0x7ULL) == bitscan(kingPosition) / 8 - (bitscan(kingPosition) & 0x7ULL) || position / 8 + (position & 0x7ULL) == bitscan(kingPosition) / 8 + (bitscan(kingPosition) & 0x7ULL)) {
            unsigned long long ray = getD12Ray(kingPosition, i&-i);
            if(__builtin_popcountll((ray - (i&-i)) & occupied) == 1) {
                board[D12] |= ray;
            }
        }
        board[ATTACK_MASK] |= moveMap;
        moveMap = ROOK_MASK[position][(ROOK_RELEVANCY_MASK[position] & occupied) * ROOK_MAGIC[position] >> (64 - ROOK_RELEVANT_BITS[position])];
        if(moveMap & kingPosition) {
            if(board[CHECKMASK] == MAX) {
                board[CHECKMASK] = getHVRay(kingPosition, i&-i);
            } else {
                board[CHECKMASK] = 0;
            }
        }
        if(position / 8 == bitscan(kingPosition) / 8 || (position & 0x7ULL) == (bitscan(kingPosition) & 0x7ULL)) {
            unsigned long long ray = getHVRay(kingPosition, i&-i);
            if(__builtin_popcountll((ray - (i&-i)) & occupied) == 1) {
                board[HV] |= ray;
            }
        }
        board[ATTACK_MASK] |= moveMap;
    }

}

inline void kingCompute(int side, unsigned long long board[]) {
    side = BLACK - side;
    int position = bitscan(board[side + KING]);
    board[ATTACK_MASK] |= KING_MASK[position];   
}

inline void computeMasks(int side, unsigned long long board[]) {
    board[CHECKMASK] = MAX;
    board[HV] = 0;
    board[D12] = 0;
    board[ATTACK_MASK] = 0;
    board[OCCUPANCY_WHITE] = occupiedMask(WHITE, board);
    board[OCCUPANCY_BLACK] = occupiedMask(BLACK, board);
    board[OCCUPANCY] = board[OCCUPANCY_WHITE] | board[OCCUPANCY_BLACK];
    pawnCompute(side, board);
    knightCompute(side, board);
    bishopCompute(side, board);
    rookCompute(side, board);
    queenCompute(side, board);
    kingCompute(side, board);
}

inline void setPosition(string fen, unsigned long long board[], int pieces[]) {
    fill(board, board + BITBOARD_SIZE, 0);
    fill(pieces, pieces + 64, -1);
    int curr = 56;
    for(int i = 0; i < fen[i] != ' '; i++, curr++) {
        if(fen[i] == ' ') {
            break;
        }
        if(fen[i] >= '1' && fen[i] <= '8') {
            curr += fen[i] - '1';
            continue;
        }
        else if(fen[i] == '/') {
            curr -= 17;
            continue;
        }
        else {
            if(fen[i] >= 'a' && fen[i] <= 'z') {
                switch(fen[i]) {
                    case 'p':
                        board[BLACK + PAWN] += 1ULL << curr;
                        pieces[curr] = PAWN;
                        break;
                    case 'n':
                        board[BLACK + KNIGHT] += 1ULL << curr;
                        pieces[curr] = KNIGHT;
                        break;
                    case 'b':
                        board[BLACK + BISHOP] += 1ULL << curr;
                        pieces[curr] = BISHOP;
                        break;
                    case 'r':
                        board[BLACK + ROOK] += 1ULL << curr;
                        pieces[curr] = ROOK;
                        break;
                    case 'q':
                        board[BLACK + QUEEN] += 1ULL << curr;
                        pieces[curr] = QUEEN;
                        break;
                    case 'k':
                        board[BLACK + KING] += 1ULL << curr;
                        pieces[curr] = KING;
                        break;
                }
            } else {
                switch(fen[i]) {
                    case 'P':
                        board[WHITE + PAWN] += 1ULL << curr;
                        pieces[curr] = PAWN;
                        break;
                    case 'N':
                        board[WHITE + KNIGHT] += 1ULL << curr;
                        pieces[curr] = KNIGHT;
                        break;
                    case 'B':
                        board[WHITE + BISHOP] += 1ULL << curr;
                        pieces[curr] = BISHOP;
                        break;
                    case 'R':
                        board[WHITE + ROOK] += 1ULL << curr;
                        pieces[curr] = ROOK;
                        break;
                    case 'Q':
                        board[WHITE + QUEEN] += 1ULL << curr;
                        pieces[curr] = QUEEN;
                        break;
                    case 'K':
                        board[WHITE + KING] += 1ULL << curr;
                        pieces[curr] = KING;
                        break;
                }
            }
        }
    }
    curr = fen.find(' ');
    int move;
    curr++;
    if(fen[curr] == 'w') {
        move = WHITE;
    } else {
        move = BLACK;
    }
    curr += 2;
    board[GAME_STATE] = 0;
    if(fen[curr] == 'K') {
        board[GAME_STATE] += 0x1ULL;
        curr++;
    }
    if(fen[curr] == 'Q') {
        board[GAME_STATE] += 0x2ULL;
        curr++;
    }
    if(fen[curr] == 'k') {
        board[GAME_STATE] += 0x4ULL;
        curr++;
    }
    if(fen[curr] == 'q') {
        board[GAME_STATE] += 0x8ULL;
        curr++;
    }

    if(fen[curr] == '-') {
        curr += 2;
    }
    if(fen[curr] == ' ') {
        curr++;
    }
    if(fen[curr] == '-') {
        curr += 2;
    } else {
        board[GAME_STATE] += ((fen[curr] - 'a' + 1) << 4);
    }
    board[GAME_STATE] += (1ULL << 32) * (move == BLACK);
    computeMasks(move, board);
    board[HASH] = zobrist(board);
    
}   

inline void pawnMoves(int side, unsigned long long board[], fixedVector<unsigned int>& moves, int pieces[]) {
    unsigned long long occupied_white = board[OCCUPANCY_WHITE];
    unsigned long long occupied_black = board[OCCUPANCY_BLACK];
    unsigned long long unoccupied = ~(board[OCCUPANCY]);
    //single push
    unsigned long long pawns = board[side + PAWN];
    pawns &= ~board[D12];
    unsigned long long pinnedPawns = pawns & board[HV];
    pawns -= pinnedPawns;
    pinnedPawns &= (side == WHITE) ? ((pinnedPawns << 8) & board[HV]) >> 8 : ((pinnedPawns >> 8) & board[HV]) << 8;
    pawns += pinnedPawns;
    unsigned long long pushed = (side == WHITE) ? (pawns << 8) : (pawns >> 8);
    pushed &= unoccupied;
    unsigned long long pushedCopy = pushed;
    pushed &= board[CHECKMASK];
    for(unsigned long long i = pushed; i != 0; i -= i&-i) {
        int to = bitscan(i&-i);
        int from = (side == WHITE) ? (to - 8) : (to + 8);
        //promotion
        if(to / 8 == ((side == WHITE) ? 7 : 0)) {
            moves.push_back(moveToInt(from, to, 3, 0, PAWN));
            moves.push_back(moveToInt(from, to, 3, 1, PAWN));
            moves.push_back(moveToInt(from, to, 3, 2, PAWN));
            moves.push_back(moveToInt(from, to, 3, 3, PAWN));
        } else {
            moves.push_back(moveToInt(from, to, 0, 0, PAWN));
        }
    }
    //double push
    pushed = (side == WHITE) ? (pushedCopy << 8) : (pushedCopy >> 8);
    pushed &= unoccupied & ((side == WHITE) ? RANK_4 : RANK_5) & board[CHECKMASK];
    for(unsigned long long i = pushed; i != 0; i -= i&-i) {
        int to = bitscan(i&-i);
        int from = (side == WHITE) ? (to - 16) : (to + 16);
        moves.push_back(moveToInt(from, to, 0, 0, PAWN));
    }


    //take right
    pawns = board[side + PAWN];
    pawns &= ~board[HV];
    pinnedPawns = pawns & board[D12];
    pawns -= pinnedPawns;
    pinnedPawns &= (side == WHITE) ? ((pinnedPawns << 9) & board[D12]) >> 9 : ((pinnedPawns >> 7) & board[D12]) << 7;
    pawns += pinnedPawns;
    unsigned long long took = (side == WHITE) ? (pawns << 9) : (pawns >> 7);
    int enPassantFile = (board[GAME_STATE] >> 4) & 0xFULL;
    //en-passant
    if(enPassantFile) {
        unsigned long long king = board[side + KING];
        int kingRank = bitscan(king) / 8;
        if(kingRank == (side == WHITE ? 4 : 3)) {
            unsigned long long rooks = board[BLACK - side + ROOK] & RANKS[kingRank];
            bool isSet = false;
            while(rooks != 0 && !isSet) {
                if(__builtin_popcountll((getHVRay(king, rooks&-rooks) - (rooks&-rooks)) & (~unoccupied)) == 2) {
                    isSet = true;
                    took &= (side == WHITE) ? occupied_black : occupied_white;
                    break;
                }
                rooks -= rooks&-rooks;
            }
            unsigned long long queens = board[BLACK - side + QUEEN] & RANKS[kingRank];
            while(queens != 0 && !isSet) {
                if(__builtin_popcountll((getHVRay(king, queens&-queens) - (queens&-queens)) & (~unoccupied)) == 2) {
                    isSet = true;
                    took &= (side == WHITE) ? occupied_black : occupied_white;
                    break;
                }
                queens -= queens&-queens;
            }
            if(!isSet) {
                int pos = (side == WHITE ? 40 : 16) + enPassantFile - 1;
                took &= (side == WHITE) ? occupied_black | (1ULL << pos) : occupied_white | (1ULL << pos);
            }
        }
        else if((1ULL << ((side == WHITE ? 32 : 24) + enPassantFile - 1)) & board[D12]) {
            took &= (side == WHITE) ? occupied_black : occupied_white;
        }
        else {
            int pos = (side == WHITE ? 40 : 16) + enPassantFile - 1;
            took &= (side == WHITE) ? occupied_black | (1ULL << pos) : occupied_white | (1ULL << pos);
        }
    }
    else took &= (side == WHITE) ? occupied_black : occupied_white;
    took &= ~A_FILE & board[CHECKMASK];
    for(unsigned long long i = took; i != 0; i -= i&-i) {
        int to = bitscan(i&-i);
        int piece = (pieces[to] != -1 ? pieces[to] : PAWN);
        int from = (side == WHITE) ? (to - 9) : (to + 7);
        //promotion
        if(to / 8 == ((side == WHITE) ? 7 : 0)) {
            moves.push_back(moveToInt(from, to, 3, 0, PAWN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[PAWN]));
            moves.push_back(moveToInt(from, to, 3, 1, PAWN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[PAWN]));
            moves.push_back(moveToInt(from, to, 3, 2, PAWN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[PAWN]));
            moves.push_back(moveToInt(from, to, 3, 3, PAWN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[PAWN]));
        } else {
            if(enPassantFile && to == ((side == WHITE ? 40 : 16) + enPassantFile - 1))
                moves.push_back(moveToInt(from, to, 2, 0, PAWN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[PAWN]));
            else moves.push_back(moveToInt(from, to, 0, 0, PAWN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[PAWN]));
        }
    }

    //take left
    pawns = board[side + PAWN];
    pawns &= ~board[HV];
    pinnedPawns = pawns & board[D12];
    pawns -= pinnedPawns;
    pinnedPawns &= (side == WHITE) ? ((pinnedPawns << 7) & board[D12]) >> 7 : ((pinnedPawns >> 9) & board[D12]) << 9;
    pawns += pinnedPawns;
    took = (side == WHITE) ? (pawns << 7) : (pawns >> 9);
    //en-passant
    if(enPassantFile) {
        unsigned long long king = board[side + KING];
        int kingRank = bitscan(king) / 8;
        if(kingRank == (side == WHITE ? 4 : 3)) {
            unsigned long long rooks = board[BLACK - side + ROOK] & RANKS[kingRank];
            bool isSet = false;
            while(rooks != 0 && !isSet) {
                if(__builtin_popcountll((getHVRay(king, rooks&-rooks) - (rooks&-rooks)) & (~unoccupied)) == 2) {
                    isSet = true;
                    took &= (side == WHITE) ? occupied_black : occupied_white;
                    break;
                }
                rooks -= rooks&-rooks;
            }
            unsigned long long queens = board[BLACK - side + QUEEN] & RANKS[kingRank];
            while(queens != 0 && !isSet) {
                if(__builtin_popcountll((getHVRay(king, queens&-queens) - (queens&-queens)) & (~unoccupied)) == 2) {
                    isSet = true;
                    took &= (side == WHITE) ? occupied_black : occupied_white;
                    break;
                }
                queens -= queens&-queens;
            }
            if(!isSet) {
                int pos = (side == WHITE ? 40 : 16) + enPassantFile - 1;
                took &= (side == WHITE) ? occupied_black | (1ULL << pos) : occupied_white | (1ULL << pos);
            }
        }
        else if((1ULL << ((side == WHITE ? 32 : 24) + enPassantFile - 1)) & board[D12]) {
            took &= (side == WHITE) ? occupied_black : occupied_white;
        } else {
            int pos = (side == WHITE ? 40 : 16) + enPassantFile - 1;
            took &= (side == WHITE) ? occupied_black | (1ULL << pos) : occupied_white | (1ULL << pos);
        }
    }
    else took &= (side == WHITE) ? occupied_black : occupied_white;
    took &= ~H_FILE & board[CHECKMASK];
    for(unsigned long long i = took; i != 0; i -= i&-i) {
        int to = bitscan(i&-i);
        int piece = (pieces[to] != -1 ? pieces[to] : PAWN);
        int from = (side == WHITE) ? (to - 7) : (to + 9);
        //promotion
        if(to / 8 == ((side == WHITE) ? 7 : 0)) {
            moves.push_back(moveToInt(from, to, 3, 0, PAWN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[PAWN]));
            moves.push_back(moveToInt(from, to, 3, 1, PAWN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[PAWN]));
            moves.push_back(moveToInt(from, to, 3, 2, PAWN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[PAWN]));
            moves.push_back(moveToInt(from, to, 3, 3, PAWN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[PAWN]));
        } else {
            if(enPassantFile && to == ((side == WHITE ? 40 : 16) + enPassantFile - 1))
                moves.push_back(moveToInt(from, to, 2, 0, PAWN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[PAWN]));
            else moves.push_back(moveToInt(from, to, 0, 0, PAWN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[PAWN]));
        }
    }
}

inline void pawnMovesCaptures(int side, unsigned long long board[], fixedVector<unsigned int>& moves, int pieces[]) {
    unsigned long long occupied_white = board[OCCUPANCY_WHITE];
    unsigned long long occupied_black = board[OCCUPANCY_BLACK];
    unsigned long long unoccupied = ~(board[OCCUPANCY]);
    //take right
    unsigned long long pawns = board[side + PAWN];
    pawns &= ~board[HV];
    unsigned long long pinnedPawns = pawns & board[D12];
    pawns -= pinnedPawns;
    pinnedPawns &= (side == WHITE) ? ((pinnedPawns << 9) & board[D12]) >> 9 : ((pinnedPawns >> 7) & board[D12]) << 7;
    pawns += pinnedPawns;
    unsigned long long took = (side == WHITE) ? (pawns << 9) : (pawns >> 7);
    int enPassantFile = (board[GAME_STATE] >> 4) & 0xFULL;
    //en-passant
    if(enPassantFile) {
        unsigned long long king = board[side + KING];
        int kingRank = bitscan(king) / 8;
        if(kingRank == (side == WHITE ? 4 : 3)) {
            unsigned long long rooks = board[BLACK - side + ROOK] & RANKS[kingRank];
            bool isSet = false;
            while(rooks != 0 && !isSet) {
                if(__builtin_popcountll((getHVRay(king, rooks&-rooks) - (rooks&-rooks)) & (~unoccupied)) == 2) {
                    isSet = true;
                    took &= (side == WHITE) ? occupied_black : occupied_white;
                    break;
                }
                rooks -= rooks&-rooks;
            }
            unsigned long long queens = board[BLACK - side + QUEEN] & RANKS[kingRank];
            while(queens != 0 && !isSet) {
                if(__builtin_popcountll((getHVRay(king, queens&-queens) - (queens&-queens)) & (~unoccupied)) == 2) {
                    isSet = true;
                    took &= (side == WHITE) ? occupied_black : occupied_white;
                    break;
                }
                queens -= queens&-queens;
            }
            if(!isSet) {
                int pos = (side == WHITE ? 40 : 16) + enPassantFile - 1;
                took &= (side == WHITE) ? occupied_black | (1ULL << pos) : occupied_white | (1ULL << pos);
            }
        }
        else if((1ULL << ((side == WHITE ? 32 : 24) + enPassantFile - 1)) & board[D12]) {
            took &= (side == WHITE) ? occupied_black : occupied_white;
        }
        else {
            int pos = (side == WHITE ? 40 : 16) + enPassantFile - 1;
            took &= (side == WHITE) ? occupied_black | (1ULL << pos) : occupied_white | (1ULL << pos);
        }
    }
    else took &= (side == WHITE) ? occupied_black : occupied_white;
    took &= ~A_FILE & board[CHECKMASK];
    for(unsigned long long i = took; i != 0; i -= i&-i) {
        int to = bitscan(i&-i);
        int piece = (pieces[to] != -1 ? pieces[to] : PAWN);;
        int from = (side == WHITE) ? (to - 9) : (to + 7);
        //promotion
        if(to / 8 == ((side == WHITE) ? 7 : 0)) {
            moves.push_back(moveToInt(from, to, 3, 0, PAWN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[PAWN]));
            moves.push_back(moveToInt(from, to, 3, 1, PAWN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[PAWN]));
            moves.push_back(moveToInt(from, to, 3, 2, PAWN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[PAWN]));
            moves.push_back(moveToInt(from, to, 3, 3, PAWN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[PAWN]));
        } else {
            if(enPassantFile && to == ((side == WHITE ? 40 : 16) + enPassantFile - 1))
                moves.push_back(moveToInt(from, to, 2, 0, PAWN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[PAWN]));
            else moves.push_back(moveToInt(from, to, 0, 0, PAWN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[PAWN]));
        }
    }

    //take left
    pawns = board[side + PAWN];
    pawns &= ~board[HV];
    pinnedPawns = pawns & board[D12];
    pawns -= pinnedPawns;
    pinnedPawns &= (side == WHITE) ? ((pinnedPawns << 7) & board[D12]) >> 7 : ((pinnedPawns >> 9) & board[D12]) << 9;
    pawns += pinnedPawns;
    took = (side == WHITE) ? (pawns << 7) : (pawns >> 9);
    //en-passant
    if(enPassantFile) {
        unsigned long long king = board[side + KING];
        int kingRank = bitscan(king) / 8;
        if(kingRank == (side == WHITE ? 4 : 3)) {
            unsigned long long rooks = board[BLACK - side + ROOK] & RANKS[kingRank];
            bool isSet = false;
            while(rooks != 0 && !isSet) {
                if(__builtin_popcountll((getHVRay(king, rooks&-rooks) - (rooks&-rooks)) & (~unoccupied)) == 2) {
                    isSet = true;
                    took &= (side == WHITE) ? occupied_black : occupied_white;
                    break;
                }
                rooks -= rooks&-rooks;
            }
            unsigned long long queens = board[BLACK - side + QUEEN] & RANKS[kingRank];
            while(queens != 0 && !isSet) {
                if(__builtin_popcountll((getHVRay(king, queens&-queens) - (queens&-queens)) & (~unoccupied)) == 2) {
                    isSet = true;
                    took &= (side == WHITE) ? occupied_black : occupied_white;
                    break;
                }
                queens -= queens&-queens;
            }
            if(!isSet) {
                int pos = (side == WHITE ? 40 : 16) + enPassantFile - 1;
                took &= (side == WHITE) ? occupied_black | (1ULL << pos) : occupied_white | (1ULL << pos);
            }
        }
        else if((1ULL << ((side == WHITE ? 32 : 24) + enPassantFile - 1)) & board[D12]) {
            took &= (side == WHITE) ? occupied_black : occupied_white;
        } else {
            int pos = (side == WHITE ? 40 : 16) + enPassantFile - 1;
            took &= (side == WHITE) ? occupied_black | (1ULL << pos) : occupied_white | (1ULL << pos);
        }
    }
    else took &= (side == WHITE) ? occupied_black : occupied_white;
    took &= ~H_FILE & board[CHECKMASK];
    for(unsigned long long i = took; i != 0; i -= i&-i) {
        int to = bitscan(i&-i);
        int piece = (pieces[to] != -1 ? pieces[to] : PAWN);
        int from = (side == WHITE) ? (to - 7) : (to + 9);
        //promotion
        if(to / 8 == ((side == WHITE) ? 7 : 0)) {
            moves.push_back(moveToInt(from, to, 3, 0, PAWN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[PAWN]));
            moves.push_back(moveToInt(from, to, 3, 1, PAWN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[PAWN]));
            moves.push_back(moveToInt(from, to, 3, 2, PAWN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[PAWN]));
            moves.push_back(moveToInt(from, to, 3, 3, PAWN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[PAWN]));
        } else {
            if(enPassantFile && to == ((side == WHITE ? 40 : 16) + enPassantFile - 1))
                moves.push_back(moveToInt(from, to, 2, 0, PAWN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[PAWN]));
            else moves.push_back(moveToInt(from, to, 0, 0, PAWN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[PAWN]));
        }
    }
}

inline void pawnMovesPromotions(int side, unsigned long long board[], fixedVector<unsigned int>& moves, int pieces[]) {
    unsigned long long unoccupied = ~(board[OCCUPANCY]);
    //single push
    unsigned long long pawns = board[side + PAWN];
    pawns &= ~board[D12];
    unsigned long long pinnedPawns = pawns & board[HV];
    pawns -= pinnedPawns;
    pinnedPawns &= (side == WHITE) ? ((pinnedPawns << 8) & board[HV]) >> 8 : ((pinnedPawns >> 8) & board[HV]) << 8;
    pawns += pinnedPawns;
    unsigned long long pushed = (side == WHITE) ? (pawns << 8) : (pawns >> 8);
    pushed &= unoccupied;
    unsigned long long pushedCopy = pushed;
    pushed &= board[CHECKMASK];
    for(unsigned long long i = pushed; i != 0; i -= i&-i) {
        int to = bitscan(i&-i);
        int from = (side == WHITE) ? (to - 8) : (to + 8);
        //promotion
        if(to / 8 == ((side == WHITE) ? 7 : 0)) {
            moves.push_back(moveToInt(from, to, 3, 0, PAWN));
            moves.push_back(moveToInt(from, to, 3, 1, PAWN));
            moves.push_back(moveToInt(from, to, 3, 2, PAWN));
            moves.push_back(moveToInt(from, to, 3, 3, PAWN));
        }
    }
}

inline void knightMoves(int side, unsigned long long board[], fixedVector<unsigned int>& moves, int pieces[]) {
    unsigned long long notSameColor = ~(side == WHITE ? board[OCCUPANCY_WHITE] : board[OCCUPANCY_BLACK]);
    unsigned long long pins = board[HV] | board[D12];
    for(unsigned long long i = board[side + KNIGHT]; i != 0; i -= i&-i) {
        if((i&-i) & pins) continue;
        int position = bitscan(i&-i);
        unsigned long long moveMap = KNIGHT_MASK[position] & notSameColor & board[CHECKMASK];
        for(unsigned long long j = moveMap; j != 0; j -= j&-j) {
            int to = bitscan(j&-j);
            int piece = pieces[to];
            if(piece != -1) {
                moves.push_back(moveToInt(position, to, 0, 0, KNIGHT, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[KNIGHT]));
            } else {
                moves.push_back(moveToInt(position, to, 0, 0, KNIGHT));
            }
        }
    }
}

inline void knightMovesCaptures(int side, unsigned long long board[], fixedVector<unsigned int>& moves, int pieces[]) {
    unsigned long long notSameColor = ~(side == WHITE ? board[OCCUPANCY_WHITE] : board[OCCUPANCY_BLACK]);
    unsigned long long pins = board[HV] | board[D12];
    for(unsigned long long i = board[side + KNIGHT]; i != 0; i -= i&-i) {
        if((i&-i) & pins) continue;
        int position = bitscan(i&-i);
        unsigned long long moveMap = KNIGHT_MASK[position] & notSameColor & board[OCCUPANCY] & board[CHECKMASK];
        for(unsigned long long j = moveMap; j != 0; j -= j&-j) {
            int to = bitscan(j&-j);
            int piece = pieces[to];
            moves.push_back(moveToInt(position, to, 0, 0, KNIGHT, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[KNIGHT]));
        }
    }
}

inline void kingMoves(int side, unsigned long long board[], fixedVector<unsigned int>& moves, int pieces[]) {
    unsigned long long notSameColor = ~(side == WHITE ? board[OCCUPANCY_WHITE] : board[OCCUPANCY_BLACK]);
    unsigned long long occupied = board[OCCUPANCY];
    int position = bitscan(board[side + KING]);
    unsigned long long moveMap = KING_MASK[position] & notSameColor & ~board[ATTACK_MASK];
    for(unsigned long long i = moveMap; i != 0; i -= i&-i) {
        int to = bitscan(i&-i);
        int piece = pieces[to];
        if(piece != -1) {
            moves.push_back(moveToInt(position, to, 0, 0, KING, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[KING]));
        } else {
            moves.push_back(moveToInt(position, to, 0, 0, KING));
        }
    }

    //kingside castle w
    if(side == WHITE && (board[GAME_STATE] & 0x1ULL) != 0) {
        if(board[CHECKMASK] == MAX && ((board[ATTACK_MASK] >> 5) & 0x1ULL) == 0 && ((board[ATTACK_MASK] >> 6) & 0x1ULL) == 0
        && ((occupied >> 5) & 0x1ULL) == 0 && ((occupied >> 6) & 0x1ULL) == 0) {
            moves.push_back(moveToInt(4, 6, 1, 0, KING));
        }
    }
    //queenside castle w
    if(side == WHITE && (board[GAME_STATE] & 0x2ULL) != 0) {
        if(board[CHECKMASK] == MAX && ((board[ATTACK_MASK] >> 2) & 0x1ULL) == 0 && ((board[ATTACK_MASK] >> 3) & 0x1ULL) == 0
        && ((occupied >> 1) & 0x1ULL) == 0 && ((occupied >> 2) & 0x1ULL) == 0 && ((occupied >> 3) & 0x1ULL) == 0) {
            moves.push_back(moveToInt(4, 2, 1, 0, KING));
        }
    }
    //kingside castle b
    if(side == BLACK && (board[GAME_STATE] & 0x4ULL) != 0) {
        if(board[CHECKMASK] == MAX && ((board[ATTACK_MASK] >> 61) & 0x1ULL) == 0 && ((board[ATTACK_MASK] >> 62) & 0x1ULL) == 0
        && ((occupied >> 61) & 0x1ULL ) == 0 && ((occupied >> 62) & 0x1ULL) == 0) {
            moves.push_back(moveToInt(60, 62, 1, 0, KING));
        }
    }
    //queenside castle b
    if(side == BLACK && (board[GAME_STATE] & 0x8ULL) != 0) {
        if(board[CHECKMASK] == MAX && ((board[ATTACK_MASK] >> 58) & 0x1ULL) == 0 && ((board[ATTACK_MASK] >> 59) & 0x1ULL) == 0
        && ((occupied >> 57) & 0x1ULL) == 0 && ((occupied >> 58) & 0x1ULL) == 0 && ((occupied >> 59) & 0x1ULL) == 0) {
            moves.push_back(moveToInt(60, 58, 1, 0, KING));
        }
    }
}

inline void kingMovesCaptures(int side, unsigned long long board[], fixedVector<unsigned int>& moves, int pieces[]) {
    unsigned long long notSameColor = ~(side == WHITE ? board[OCCUPANCY_WHITE] : board[OCCUPANCY_BLACK]);
    unsigned long long occupied = board[OCCUPANCY];
    int position = bitscan(board[side + KING]);
    unsigned long long moveMap = KING_MASK[position] & notSameColor & board[OCCUPANCY] & ~board[ATTACK_MASK];
    for(unsigned long long i = moveMap; i != 0; i -= i&-i) {
        int to = bitscan(i&-i);
        int piece = pieces[to];
        moves.push_back(moveToInt(position, to, 0, 0, KING, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[KING]));
    }
}

inline void bishopMoves(int side, unsigned long long board[], fixedVector<unsigned int>& moves, int pieces[]) {
    unsigned long long occupied = board[OCCUPANCY];
    unsigned long long notSameColor = ~(side == WHITE ? board[OCCUPANCY_WHITE] : board[OCCUPANCY_BLACK]);
    for(unsigned long long i = board[side + BISHOP]; i != 0; i -= i&-i) {
        if((i&-i) & board[HV]) continue;
        int position = bitscan(i&-i);
        unsigned long long moveMap = BISHOP_MASK[position][(BISHOP_RELEVANCY_MASK[position] & occupied) * BISHOP_MAGIC[position] >> (64 - BISHOP_RELEVANT_BITS[position])] & notSameColor & board[CHECKMASK];
        if((i&-i) & board[D12]) {
            moveMap &= board[D12];
        }
        for(unsigned long long i = moveMap; i != 0; i -= i&-i) {
            int to = bitscan(i&-i);
            int piece = pieces[to];
            if(piece != -1) {
                moves.push_back(moveToInt(position, to, 0, 0, BISHOP, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[BISHOP]));
            } else {
                moves.push_back(moveToInt(position, to, 0, 0, BISHOP));
            }
        }
    }
}

inline void bishopMovesCaptures(int side, unsigned long long board[], fixedVector<unsigned int>& moves, int pieces[]) {
    unsigned long long occupied = board[OCCUPANCY];
    unsigned long long notSameColor = ~(side == WHITE ? board[OCCUPANCY_WHITE] : board[OCCUPANCY_BLACK]);
    for(unsigned long long i = board[side + BISHOP]; i != 0; i -= i&-i) {
        if((i&-i) & board[HV]) continue;
        int position = bitscan(i&-i);
        unsigned long long moveMap = BISHOP_MASK[position][(BISHOP_RELEVANCY_MASK[position] & occupied) * BISHOP_MAGIC[position] >> (64 - BISHOP_RELEVANT_BITS[position])] & notSameColor & board[OCCUPANCY] & board[CHECKMASK];
        if((i&-i) & board[D12]) {
            moveMap &= board[D12];
        }
        for(unsigned long long i = moveMap; i != 0; i -= i&-i) {
            int to = bitscan(i&-i);
            int piece = pieces[to];
            moves.push_back(moveToInt(position, to, 0, 0, BISHOP, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[BISHOP]));
        }
    }
}

inline void rookMoves(int side, unsigned long long board[], fixedVector<unsigned int>& moves, int pieces[]) {
    unsigned long long occupied = board[OCCUPANCY];
    unsigned long long notSameColor = ~(side == WHITE ? board[OCCUPANCY_WHITE] : board[OCCUPANCY_BLACK]);
    for(unsigned long long i = board[side + ROOK]; i != 0; i -= i&-i) {
        if((i&-i) & board[D12]) continue;
        int position = bitscan(i&-i);
        unsigned long long moveMap = ROOK_MASK[position][(ROOK_RELEVANCY_MASK[position] & occupied) * ROOK_MAGIC[position] >> (64 - ROOK_RELEVANT_BITS[position])] & notSameColor & board[CHECKMASK];
        if((i&-i) & board[HV]) {
            moveMap &= board[HV];
        }
        for(unsigned long long i = moveMap; i != 0; i -= i&-i) {
            int to = bitscan(i&-i);
            int piece = pieces[to];
            if(piece != -1) {
                moves.push_back(moveToInt(position, to, 0, 0, ROOK, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[ROOK]));
            } else {
                moves.push_back(moveToInt(position, to, 0, 0, ROOK));
            }
        }
    }
}

inline void rookMovesCaptures(int side, unsigned long long board[], fixedVector<unsigned int>& moves, int pieces[]) {
    unsigned long long occupied = board[OCCUPANCY];
    unsigned long long notSameColor = ~(side == WHITE ? board[OCCUPANCY_WHITE] : board[OCCUPANCY_BLACK]);
    for(unsigned long long i = board[side + ROOK]; i != 0; i -= i&-i) {
        if((i&-i) & board[D12]) continue;
        int position = bitscan(i&-i);
        unsigned long long moveMap = ROOK_MASK[position][(ROOK_RELEVANCY_MASK[position] & occupied) * ROOK_MAGIC[position] >> (64 - ROOK_RELEVANT_BITS[position])] & notSameColor & board[OCCUPANCY] & board[CHECKMASK];
        if((i&-i) & board[HV]) {
            moveMap &= board[HV];
        }
        for(unsigned long long i = moveMap; i != 0; i -= i&-i) {
            int to = bitscan(i&-i);
            int piece = pieces[to];
            moves.push_back(moveToInt(position, to, 0, 0, ROOK, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[ROOK]));
        }
    }
}

inline void queenMoves(int side, unsigned long long board[], fixedVector<unsigned int>& moves, int pieces[]) {
    unsigned long long occupied = board[OCCUPANCY];
    unsigned long long notSameColor = ~(side == WHITE ? board[OCCUPANCY_WHITE] : board[OCCUPANCY_BLACK]);
    for(unsigned long long i = board[side + QUEEN]; i != 0; i -= i&-i) {
        int position = bitscan(i&-i);
        unsigned long long moveMap;
        if(!((i&-i) & board[HV])) {
            moveMap = BISHOP_MASK[position][(BISHOP_RELEVANCY_MASK[position] & occupied) * BISHOP_MAGIC[position] >> (64 - BISHOP_RELEVANT_BITS[position])] & notSameColor & board[CHECKMASK];
            if((i&-i) & board[D12]) {
                moveMap &= board[D12];
            }
            for(unsigned long long i = moveMap; i != 0; i -= i&-i) {
                int to = bitscan(i&-i);
                int piece = pieces[to];
                if(piece != -1) {
                    moves.push_back(moveToInt(position, to, 0, 0, QUEEN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[QUEEN]));
                } else {
                    moves.push_back(moveToInt(position, to, 0, 0, QUEEN));
                }
            }
        }
        if(!((i&-i) & board[D12])) {
            moveMap = ROOK_MASK[position][(ROOK_RELEVANCY_MASK[position] & occupied) * ROOK_MAGIC[position] >> (64 - ROOK_RELEVANT_BITS[position])] & notSameColor & board[CHECKMASK];
            if((i&-i) & board[HV]) {
                moveMap &= board[HV];
            }
            for(unsigned long long i = moveMap; i != 0; i -= i&-i) {
                int to = bitscan(i&-i);
                int piece = pieces[to];
                if(piece != -1) {
                    moves.push_back(moveToInt(position, to, 0, 0, QUEEN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[QUEEN]));
                } else {
                    moves.push_back(moveToInt(position, to, 0, 0, QUEEN));
                }
            }
        }
    }
}

inline void queenMovesCaptures(int side, unsigned long long board[], fixedVector<unsigned int>& moves, int pieces[]) {
    unsigned long long occupied = board[OCCUPANCY];
    unsigned long long notSameColor = ~(side == WHITE ? board[OCCUPANCY_WHITE] : board[OCCUPANCY_BLACK]);
    for(unsigned long long i = board[side + QUEEN]; i != 0; i -= i&-i) {
        int position = bitscan(i&-i);
        unsigned long long moveMap;
        if(!((i&-i) & board[HV])) {
            moveMap = BISHOP_MASK[position][(BISHOP_RELEVANCY_MASK[position] & occupied) * BISHOP_MAGIC[position] >> (64 - BISHOP_RELEVANT_BITS[position])] & notSameColor & board[OCCUPANCY] & board[CHECKMASK];
            if((i&-i) & board[D12]) {
                moveMap &= board[D12];
            }
            for(unsigned long long i = moveMap; i != 0; i -= i&-i) {
                int to = bitscan(i&-i);
                int piece = pieces[to];
                moves.push_back(moveToInt(position, to, 0, 0, QUEEN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[QUEEN]));
            }
        }
        if(!((i&-i) & board[D12])) {
            moveMap = ROOK_MASK[position][(ROOK_RELEVANCY_MASK[position] & occupied) * ROOK_MAGIC[position] >> (64 - ROOK_RELEVANT_BITS[position])] & notSameColor & board[OCCUPANCY] & board[CHECKMASK];
            if((i&-i) & board[HV]) {
                moveMap &= board[HV];
            }
            for(unsigned long long i = moveMap; i != 0; i -= i&-i) {
                int to = bitscan(i&-i);
                int piece = pieces[to];
                moves.push_back(moveToInt(position, to, 0, 0, QUEEN, 8 * (PIECE_WEIGHTS[piece] + 1) - PIECE_WEIGHTS[QUEEN]));
            }
        }
    }
}

inline void makeMove(unsigned int move, unsigned long long board[], int pieces[]) {
    unsigned long long from = 1ULL << (move & 0x3FULL);
    unsigned long long to = 1ULL << ((move >> 6) & 0x3FULL);
    int special = (move >> 12) & 0x3ULL;
    int promotion = (move >> 14) & 0x3ULL;
    int side = getSide(board);
    int piece = ((move >> 16) & 0x7ULL) + side;
    bool capture = (side == WHITE ? board[OCCUPANCY_BLACK] : board[OCCUPANCY_WHITE]) & to;
    if(capture) {        
        int taken = BLACK - side + pieces[bitscan(to)];
        board[taken] -= to;
        pieces[bitscan(to)] = taken - side;
        board[HASH] ^= ZOBRIST_BITSTRINGS[bitscan(to)][taken];
    }
    if(special != 3) {
        if(special == 0) {
            //if king or rook moved, revoke castle ability
            if(piece - side == KING) {
                if(side == WHITE) {
                    board[GAME_STATE] = setBit(board[GAME_STATE], 0, 0);
                    board[GAME_STATE] = setBit(board[GAME_STATE], 1, 0);
                } 
                if(side == BLACK) {
                    board[GAME_STATE] = setBit(board[GAME_STATE], 2, 0);
                    board[GAME_STATE] = setBit(board[GAME_STATE], 3, 0);
                }
            }
            int fromSquare = bitscan(from);
            if(piece - side == ROOK) {
                if(side == WHITE && (board[GAME_STATE] & 0x2ULL) != 0 && fromSquare == 0) {
                    board[GAME_STATE] = setBit(board[GAME_STATE], 1, 0);
                } 
                if(side == WHITE && (board[GAME_STATE] & 0x1ULL) != 0 && fromSquare == 7) {
                    board[GAME_STATE] = setBit(board[GAME_STATE], 0, 0);
                } 
                if(side == BLACK && (board[GAME_STATE] & 0x8ULL) != 0 && fromSquare == 56) {
                    board[GAME_STATE] = setBit(board[GAME_STATE], 3, 0);
                }
                if(side == BLACK && (board[GAME_STATE] & 0x4ULL) != 0 && fromSquare == 63) {
                    board[GAME_STATE] = setBit(board[GAME_STATE], 2, 0);
                }
            }
        }
        board[piece] -= from;
        pieces[bitscan(from)] = -1;
        board[HASH] ^= ZOBRIST_BITSTRINGS[bitscan(from)][piece];
        int toSquare = bitscan(to);
        //if a rook is taken revoke castle ability
        if(side == BLACK && (board[GAME_STATE] & 0x2ULL) != 0 && toSquare == 0) {
            board[GAME_STATE] = setBit(board[GAME_STATE], 1, 0);
        } 
        if(side == BLACK && (board[GAME_STATE] & 0x1ULL) != 0 && toSquare == 7) {
            board[GAME_STATE] = setBit(board[GAME_STATE], 0, 0);
        } 
        if(side == WHITE && (board[GAME_STATE] & 0x8ULL) != 0 && toSquare == 56) {
            board[GAME_STATE] = setBit(board[GAME_STATE], 3, 0);
        }
        if(side == WHITE && (board[GAME_STATE] & 0x4ULL) != 0 && toSquare == 63) {
            board[GAME_STATE] = setBit(board[GAME_STATE], 2, 0);
        }
        board[piece] += to;
        pieces[bitscan(to)] = piece - side;
        board[HASH] ^= ZOBRIST_BITSTRINGS[bitscan(to)][piece];
        if(special == 1) {
            if(side == WHITE) {
                if(toSquare == 6) {
                    board[WHITE + ROOK] -= (1ULL << 7);
                    pieces[7] = -1;
                    board[WHITE + ROOK] += (1ULL << 5);
                    pieces[5] = ROOK;
                    board[HASH] ^= ZOBRIST_BITSTRINGS[7][WHITE + ROOK];
                    board[HASH] ^= ZOBRIST_BITSTRINGS[5][WHITE + ROOK];
                    board[GAME_STATE] = setBit(board[GAME_STATE], 0, 0);
                    board[GAME_STATE] = setBit(board[GAME_STATE], 1, 0);
                } else {
                    board[WHITE + ROOK] -= 1;
                    pieces[0] = -1; 
                    board[WHITE + ROOK] += (1ULL << 3);
                    pieces[3] = ROOK;
                    board[HASH] ^= ZOBRIST_BITSTRINGS[0][WHITE + ROOK];
                    board[HASH] ^= ZOBRIST_BITSTRINGS[3][WHITE + ROOK];
                    board[GAME_STATE] = setBit(board[GAME_STATE], 0, 0);
                    board[GAME_STATE] = setBit(board[GAME_STATE], 1, 0);
                }
            } else {
                if(toSquare == 62) {
                    board[BLACK + ROOK] -= (1ULL << 63);
                    pieces[63] = -1;
                    board[BLACK + ROOK] += (1ULL << 61);
                    pieces[61] = ROOK;
                    board[HASH] ^= ZOBRIST_BITSTRINGS[63][BLACK + ROOK];
                    board[HASH] ^= ZOBRIST_BITSTRINGS[61][BLACK + ROOK];
                    board[GAME_STATE] = setBit(board[GAME_STATE], 2, 0);
                    board[GAME_STATE] = setBit(board[GAME_STATE], 3, 0);
                } else {
                    board[BLACK + ROOK] -= (1ULL << 56);
                    pieces[56]= -1;
                    board[BLACK + ROOK] += (1ULL << 59);
                    pieces[59] = ROOK;
                    board[HASH] ^= ZOBRIST_BITSTRINGS[56][BLACK + ROOK];
                    board[HASH] ^= ZOBRIST_BITSTRINGS[59][BLACK + ROOK];
                    board[GAME_STATE] = setBit(board[GAME_STATE], 2, 0);
                    board[GAME_STATE] = setBit(board[GAME_STATE], 3, 0);
                }
            }
        }
        if(special == 2) {
            unsigned long long pawn = (side == WHITE ? (to >> 8) : (to << 8));
            board[BLACK - side + PAWN] -= pawn;
            pieces[bitscan(pawn)] = -1;
            board[HASH] ^= ZOBRIST_BITSTRINGS[bitscan(pawn)][BLACK - side + PAWN];
        }
    } else {
        board[piece] -= from;
        pieces[bitscan(from)] = -1;
        board[HASH] ^= ZOBRIST_BITSTRINGS[bitscan(from)][piece];
        board[1 + side + promotion] += to;
        pieces[bitscan(to)] = 1 + promotion;
        board[HASH] ^= ZOBRIST_BITSTRINGS[bitscan(to)][1 + side + promotion];
        int toSquare = bitscan(to);
        if(side == BLACK && (board[GAME_STATE] & 0x2ULL) != 0 && toSquare == 0) {
            board[GAME_STATE] = setBit(board[GAME_STATE], 1, 0);
        } 
        if(side == BLACK && (board[GAME_STATE] & 0x1ULL) != 0 && toSquare == 7) {
            board[GAME_STATE] = setBit(board[GAME_STATE], 0, 0);
        } 
        if(side == WHITE && (board[GAME_STATE] & 0x8ULL) != 0 && toSquare == 56) {
            board[GAME_STATE] = setBit(board[GAME_STATE], 3, 0);
        }
        if(side == WHITE && (board[GAME_STATE] & 0x4ULL) != 0 && toSquare == 63) {
            board[GAME_STATE] = setBit(board[GAME_STATE], 2, 0);
        }
    }
    if(piece - side == PAWN && abs(bitscan(to) - bitscan(from)) == 16) {
        board[GAME_STATE] &= ~(0xF0);
        board[GAME_STATE] += ((1 + (bitscan(from) & 0x7ULL)) << 4);
    } else {
        board[GAME_STATE] &= ~(0xF0);
    }
    side = BLACK - side;
    computeMasks(side, board);
    board[GAME_STATE] = setBit(board[GAME_STATE], 32, (side == BLACK));
    board[HASH] ^= ZOBRIST_BLACK_TO_MOVE;
}

inline void generateMoves(int side, unsigned long long board[], fixedVector<unsigned int>& moves, int pieces[]) {
    pawnMoves(side, board, moves, pieces);
    knightMoves(side, board, moves, pieces);
    bishopMoves(side, board, moves, pieces);
    rookMoves(side, board, moves, pieces);
    queenMoves(side, board, moves, pieces);
    kingMoves(side, board, moves, pieces);
}

inline void generateCaptures(int side, unsigned long long board[], fixedVector<unsigned int>& moves, int pieces[]) {
    pawnMovesCaptures(side, board, moves, pieces);
    pawnMovesPromotions(side, board, moves, pieces);
    knightMovesCaptures(side, board, moves, pieces);
    bishopMovesCaptures(side, board, moves, pieces);
    rookMovesCaptures(side, board, moves, pieces);
    queenMovesCaptures(side, board, moves, pieces);
    kingMovesCaptures(side, board, moves, pieces);
}

inline unsigned long long perft(int depth, unsigned long long board[], int pieces[], bool debug=false) {
    int side = getSide(board);
    fixedVector<unsigned int> moves;
    generateMoves(side, board, moves, pieces);
    unsigned long long nodes = 0;
    if(depth <= 1) {
        return moves.size; 
    }
    for(int i = 0; i < moves.size; i++) {
        unsigned int curr = moves.arr[i];
        if(debug) cout << intToCoord(curr & 0x3F) << intToCoord((curr >> 6) & 0x3F) << ": ";
        unsigned long long boardcpy[BITBOARD_SIZE];
        int piececpy[64];
        copy(board, board + BITBOARD_SIZE, boardcpy);
        copy(pieces, pieces + 64, piececpy);
        makeMove(curr, boardcpy, piececpy);
        unsigned long long current = perft(depth - 1, boardcpy, piececpy);
        if(debug) cout << current << "\n";
        nodes += current;
    }
    return nodes;
}

inline unsigned long long perft(int depth, string fen) {
    setPosition(fen, bitboard, piece_board);
    #ifdef DEBUG
    return perft(depth, bitboard, true);
    #else
    return perft(depth, bitboard, piece_board, false);
    #endif
}

double evaluate(unsigned long long board[]) {
    double result = 0;
    result += __builtin_popcountll(board[WHITE + PAWN]) - __builtin_popcountll(board[BLACK + PAWN]);
    result += 2.7 * __builtin_popcountll(board[WHITE + KNIGHT]) - 2.7 * __builtin_popcountll(board[BLACK + KNIGHT]);
    result += 2.9 * __builtin_popcountll(board[WHITE + BISHOP]) - 2.9 * __builtin_popcountll(board[BLACK + BISHOP]);
    result += 4.9 * __builtin_popcountll(board[WHITE + ROOK]) - 4.9 * __builtin_popcountll(board[BLACK + ROOK]);
    result += 9.6 * __builtin_popcountll(board[WHITE + QUEEN]) - 9.6 * __builtin_popcountll(board[BLACK + QUEEN]);
    result = round(result * 100) / 100.;
    return result;
}

double quiescenceSearch(double alpha, double beta, unsigned long long board[], int pieces[]) {
    int side = getSide(board);
    double best = evaluate(board) * (side == WHITE ? 1 : -1);
    computeMasks(side, board);
    bool isCheck = (board[CHECKMASK] != MAX);

    if(!isCheck) {
        if(best >= beta) return best;
        if(best > alpha) alpha = best;
    } else {
        best = -20000;
    }

    fixedVector<unsigned int> moves;
    if(isCheck) {
        generateMoves(side, board, moves, pieces);
        if(moves.size == 0) {
            if(board[CHECKMASK] == MAX) return 0;
            else return -10000;
        }
        moves.sort();
    } else {
        generateCaptures(side, board, moves, pieces);
        moves.sort();
    }

    for(int i = 0; i < moves.size; i++) {
        unsigned int curr = moves.arr[i];
        unsigned long long boardcpy[BITBOARD_SIZE];
        int piececpy[64];
        copy(board, board + BITBOARD_SIZE, boardcpy);
        copy(pieces, pieces + 64, piececpy);
        makeMove(curr, boardcpy, piececpy);
        double currValue = -quiescenceSearch(-beta, -alpha, boardcpy, piececpy);
        if(best < currValue) {
            best = currValue;
        }
        alpha = max(alpha, best);
        if(alpha >= beta) break;
    }

    return best;
}

pair<unsigned int, double> negamax(int depth, double alpha, double beta, unsigned long long board[], int pieces[]) {
    int side = getSide(board);
    if(depth == 0) {
        return {0, quiescenceSearch(alpha, beta, board, pieces)};
    }

    fixedVector<unsigned int> moves;
    generateMoves(side, board, moves, pieces);
    moves.sort();

    if(moves.size == 0) {
        if(board[CHECKMASK] == MAX) return {0, 0};
        else return {0, -10000};
    }
    unsigned int current;
    double value = -20000;
    for(int i = 0; i < moves.size; i++) {
        unsigned int curr = moves.arr[i];
        unsigned long long boardcpy[BITBOARD_SIZE];
        int piececpy[64];
        copy(board, board + BITBOARD_SIZE, boardcpy);
        copy(pieces, pieces + 64, piececpy);
        makeMove(curr, boardcpy, piececpy);
        pair<unsigned int, double> currMove = negamax(depth - 1, -beta, -alpha, boardcpy, piececpy);
        if(value < -currMove.second) {
            value = -currMove.second;
            current = curr;
        }
        alpha = max(alpha, value);
        if(alpha >= beta) break;
    }
    return {current, value};
}