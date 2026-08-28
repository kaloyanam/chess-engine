#include "constants.h"
using namespace std;

template<typename T>
struct fixedVector {
    T arr[256];
    int size = 0;
    void push_back(T i) {arr[size++] = i;}
    void clear() {size = 0;}
    vector<T> toVector() {return vector<T>(arr, arr + size);}
    void sort() {
        for(int i = 0; i < size; i++) {
            int best = (arr[i] >> 19) & 0x7F;
            for(int j = i + 1; j < size; j++) {
                if(best < ((arr[j] >> 19) & 0x7F)) {
                    best = (arr[j] >> 19) & 0x7F;
                    swap(arr[i], arr[j]);
                }
            }
        }
    }
};

struct historyVector {
    unsigned long long arr[MAX_HISTORY];
    int size = 0;
    void push(unsigned long long hash) {arr[size++] = hash;}
    void pop() {size--;}
};

struct TTEntry {
    unsigned long long hash;
    unsigned int move;
    int depth;
    int score;
    int flag;
    TTEntry() {
        this->hash = 0;
        this->move = 0;
        this->depth = 0;
        this->score = 0;
        this->flag = 0;
    }
    TTEntry(unsigned long long hash, unsigned int move, int depth, int score, int flag) {
        this->hash = hash;
        this->move = move;
        this->depth = depth;
        this->score = score;
        this->flag = flag;
    }
};

struct TTMap {
    TTEntry* arr;
    TTMap() {
        arr = new TTEntry[TT_SIZE];
    }
    void insert(TTEntry entry) {
        arr[entry.hash & (TT_SIZE - 1)] = entry; 
    }
    TTEntry& get(unsigned long long hash) {
        return arr[hash & (TT_SIZE - 1)];
    }
};

struct searchResult {
    unsigned int bestMove;
    int score;
    int maxDepth;
    searchResult(unsigned int bestMove, int score, int maxDepth) {
        this->bestMove = bestMove;
        this->score = score;
        this->maxDepth = maxDepth;
    }
};

extern historyVector positions;
inline void pawnMoves(int side, unsigned long long board[], fixedVector<unsigned int>& moves, int pieces[]);
inline void knightMoves(int side, unsigned long long board[], fixedVector<unsigned int>& moves, int pieces[]);
inline void bishopMoves(int side, unsigned long long board[], fixedVector<unsigned int>& moves, int pieces[]);
inline void rookMoves(int side, unsigned long long board[], fixedVector<unsigned int>& moves, int pieces[]);
inline void queenMoves(int side, unsigned long long board[], fixedVector<unsigned int>& moves, int pieces[]);
inline void kingMoves(int side, unsigned long long board[], fixedVector<unsigned int>& moves, int pieces[]);
inline void generateMoves(int side, unsigned long long board[], fixedVector<unsigned int>& moves, int pieces[]);
inline void makeMove(unsigned int move, unsigned long long board[], int pieces[]);

inline int coordToInt(string coord) {
    return coord[0] - 'a' + (coord[1] - '1') * 8;
}

inline unsigned long long currentMillis() {
    return chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count();
}

inline string intToCoord(int coord) {
    string result = "";
    result += 'a' + (coord & 0x7ULL);
    result += '1' + coord / 8;
    return result;
}

inline string bin(unsigned long long n) {
    bitset<64> a(n);
    string str = a.to_string();
    for(int i = 0, j = 0; i < str.length(); i++, j++) {
        if(j == 8) {
            str.insert(i++, "\n");
            j = 0;
        }
        if(j < 4) {
            swap(str[i], str[i + 7 - 2 * j]);
            if(str[i] == '0') str[i] = '.';
            if(str[i + 7 - 2 * j] == '0') str[i + 7 - 2 * j] = '.';
        }
    }
    return str;
}

inline unsigned long long random64() {
    static default_random_engine rng(currentMillis());
    static uniform_int_distribution<unsigned long long> distr;
    return distr(rng);
}

inline constexpr int getSide(unsigned long long board[]) {
    return ((board[GAME_STATE] >> 32) & 0x1ULL) * BLACK;
}

inline constexpr int bitscan (unsigned long long n) {
    const unsigned long long debruijn64 = 0x07EDD5E59A4E28C2ULL;
    return INDEX64[((n & -n) * debruijn64) >> 58];
}

inline void printMove(unsigned int move) {
    cout << intToCoord(move & 0x3F) << intToCoord((move >> 6) & 0x3F);
}

inline unsigned int moveToInt(int origin, int destination, int special, int promotion, int piece) {
    unsigned int result = piece;
    result <<= 2;
    result += promotion;
    result <<= 2;
    result += special;
    result <<= 6;
    result += destination;
    result <<= 6;
    result += origin;
    return result;
}

inline unsigned int moveToInt(int origin, int destination, int special, int promotion, int piece, unsigned int mvv_lva) {
    unsigned result = moveToInt(origin, destination, special, promotion, piece);
    result += mvv_lva << 19;
    return result;
}

inline unsigned int moveToInt(string move, unsigned long long board[], int pieces[]) {
    try {
        move.erase(remove(move.begin(), move.end(), '+'), move.end());
        move.erase(remove(move.begin(), move.end(), '#'), move.end());
        int side = getSide(board);
        if(move == "O-O") {
            fixedVector<unsigned int> moves;
            kingMoves(side, board, moves, pieces);
            unsigned int finalMove = side == WHITE ? moveToInt(coordToInt("e1"), coordToInt("g1"), 1, 0, 5) : moveToInt(coordToInt("e8"), coordToInt("g8"), 1, 0, 5);
            for(int i = 0; i < moves.size; i++) {
                if(moves.arr[i] == finalMove) return finalMove;
            }
            return -1;
        }
        if(move == "O-O-O") {
            fixedVector<unsigned int> moves;
            kingMoves(side, board, moves, pieces);
            unsigned int finalMove = side == WHITE ? moveToInt(coordToInt("e1"), coordToInt("c1"), 1, 0, 5) : moveToInt(coordToInt("e8"), coordToInt("c8"), 1, 0, 5);
            for(int i = 0; i < moves.size; i++) {
                if(moves.arr[i] == finalMove) return finalMove;
            }
            return -1;
        }
        fixedVector<unsigned int> validMoves;
        switch (move[0])
        {
        case 'N':
            knightMoves(side, board, validMoves, pieces);
            move = move.substr(1);
            break;
        case 'B':
            bishopMoves(side, board, validMoves, pieces);
            move = move.substr(1);
            break;
        case 'R':
            rookMoves(side, board, validMoves, pieces);
            move = move.substr(1);
            break;
        case 'Q':
            queenMoves(side, board, validMoves, pieces);
            move = move.substr(1);
            break;
        case 'K':
            kingMoves(side, board, validMoves, pieces);
            move = move.substr(1);
            break;
        default:
            pawnMoves(side, board, validMoves, pieces);
            break;
        }
        if(find(move.begin(), move.end(), 'x') != move.end()) {
            move.erase(find(move.begin(), move.end(), 'x'));
        }
        int promotionPiece = -1;
        if(find(move.begin(), move.end(), '=') != move.end()) {
            switch(move[move.size() - 1]) {
            case 'N':
                promotionPiece = 0;
                break;
            case 'B':
                promotionPiece = 1;
                break;
            case 'R':
                promotionPiece = 2;
                break;
            case 'Q':
            default:
                promotionPiece = 3;
            }
            move.erase(find(move.begin(), move.end(), '='), move.end());
        }
        int destination = coordToInt(move.substr(move.size() - 2));
        fixedVector<unsigned int> candidates;
        for(int i = 0; i < validMoves.size; i++) {
            if(((validMoves.arr[i] >> 6) & 0x3f) == destination) {
                if(promotionPiece != -1 && ((validMoves.arr[i] >> 14) & 0x3) != promotionPiece) continue;
                candidates.push_back(validMoves.arr[i]);
            }
        }
        if(candidates.size == 0) return -1;
        if(candidates.size == 1) return candidates.arr[0];
        else {
            unsigned int finalMove = -1;
            move.erase(move.size() - 2);
            if(move.size() == 0) return -1;
            if(move.size() == 1) {
                if(isalpha(move[0])) {
                    for(int i = 0; i < candidates.size; i++) {
                        if((candidates.arr[i] & 0x3f) % 8 == move[0] - 'a') {
                            if(finalMove != -1) return -1;
                            else finalMove = candidates.arr[i];
                        }
                    }
                }
                if(isdigit(move[0])) {
                    for(int i = 0; i < candidates.size; i++) {
                        if((candidates.arr[i] & 0x3f) / 8 == move[0] - '1') {
                            if(finalMove != -1) return -1;
                            else finalMove = candidates.arr[i];
                        }
                    }
                }
            } else {
                int from = coordToInt(move);
                for(int i = 0; i < candidates.size; i++) {
                        if((candidates.arr[i] & 0x3f) == from) {
                            return candidates.arr[i];
                        }
                    }
            }
            return finalMove;
        }
    } catch(...) {
        return -1;
    }
}

inline string intToMove(unsigned int move, unsigned long long board[], int pieces[]) {
    unsigned long long from = 1ULL << (move & 0x3FULL);
    unsigned long long to = 1ULL << ((move >> 6) & 0x3FULL);
    int special = (move >> 12) & 0x3ULL;
    int promotion = (move >> 14) & 0x3ULL;
    int side = getSide(board);
    bool takes = (side == WHITE ? board[OCCUPANCY_BLACK] : board[OCCUPANCY_WHITE]) & 1ULL << ((move >> 6) & 0x3FULL) || special == 2;
    from = bitscan(from);
    to = bitscan(to);
    string result = "";
    if(special == 1) {
        if(to == coordToInt("g1") || to == coordToInt("g8")) {
            result = "O-O";
            unsigned long long boardcpy[BITBOARD_SIZE];
            int piececpy[64];
            copy(board, board + BITBOARD_SIZE, boardcpy);
            copy(pieces, pieces + 64, piececpy);
            makeMove(move, boardcpy, piececpy);
            positions.pop();
            if(boardcpy[CHECKMASK] != MAX) {
                fixedVector<unsigned int> moves;
                generateMoves(BLACK - side, boardcpy, moves, piececpy);
                if(moves.size == 0) {
                    result += "#";
                } else {
                    result += "+";
                }
            }
            return result;
        }
        else {
            result = "O-O-O";
            unsigned long long boardcpy[BITBOARD_SIZE];
            int piececpy[64];
            copy(board, board + BITBOARD_SIZE, boardcpy);
            copy(pieces, pieces + 64, piececpy);
            makeMove(move, boardcpy, piececpy);
            positions.pop();
            if(boardcpy[CHECKMASK] != MAX) {
                fixedVector<unsigned int> moves;
                generateMoves(BLACK - side, boardcpy, moves, piececpy);
                if(moves.size == 0) {
                    result += "#";
                } else {
                    result += "+";
                }
            }
            return result;
        }
    }

    fixedVector<unsigned int> moves;
    switch((move >> 16) & 0x7ULL) {
    case PAWN:
        if(takes) {
            result += intToCoord(from)[0];
        }
        break;
    case KNIGHT: {
        result += "N";
        knightMoves(side, board, moves, pieces);
        vector<string> squares;
        for(int i = 0; i < moves.size; i++) {
            unsigned long long dest = bitscan(1ULL << ((moves.arr[i] >> 6) & 0x3FULL));
            if(dest == to) {
                squares.push_back(intToCoord(bitscan(1ULL << (moves.arr[i] & 0x3FULL))));
            }
        }
        if(squares.size() > 1) {
            bool sameFile = false;
            bool sameRank = false;
            string source = intToCoord(from);
            char file = source[0];
            char rank = source[1];
            for(int i = 0; i < squares.size(); i++) {
                if(squares.at(i) == source) continue;
                if(squares.at(i)[0] == file) {
                    sameFile = true;
                }
                if(squares.at(i)[1] == rank) {
                    sameRank = true;
                }
            }
            if(sameFile == false ) {
                result += intToCoord(from)[0];
            }
            else if(sameRank == false) {
                result += intToCoord(from)[1];
            }
            else {
                result += intToCoord(from);
            }
         }
         break;
    }
    case BISHOP: {
        result += "B";
        bishopMoves(side, board, moves, pieces);
        vector<string> squares;
        for(int i = 0; i < moves.size; i++) {
            unsigned long long dest = bitscan(1ULL << ((moves.arr[i] >> 6) & 0x3FULL));
            if(dest == to) {
                squares.push_back(intToCoord(bitscan(1ULL << (moves.arr[i] & 0x3FULL))));
            }
        }
        if(squares.size() > 1) {
            bool sameFile = false;
            bool sameRank = false;
            string source = intToCoord(from);
            char file = source[0];
            char rank = source[1];
            for(int i = 0; i < squares.size(); i++) {
                if(squares.at(i) == source) continue;
                if(squares.at(i)[0] == file) {
                    sameFile = true;
                }
                if(squares.at(i)[1] == rank) {
                    sameRank = true;
                }
            }
            if(sameFile == false ) {
                result += intToCoord(from)[0];
            }
            else if(sameRank == false) {
                result += intToCoord(from)[1];
            }
            else {
                result += intToCoord(from);
            }
         }
         break;
    }
    case ROOK: {
        result += "R";
        rookMoves(side, board, moves, pieces);
        vector<string> squares;
        for(int i = 0; i < moves.size; i++) {
            unsigned long long dest = bitscan(1ULL << ((moves.arr[i] >> 6) & 0x3FULL));
            if(dest == to) {
                squares.push_back(intToCoord(bitscan(1ULL << (moves.arr[i] & 0x3FULL))));
            }
        }
        if(squares.size() > 1) {
            bool sameFile = false;
            bool sameRank = false;
            char file = squares.at(0)[0];
            char rank = squares.at(0)[1];
            for(int i = 1; i < squares.size(); i++) {
                if(squares.at(i)[0] != file) {
                    sameFile = true;
                }
                if(squares.at(i)[1] != rank) {
                    sameRank = true;
                }
            }
            if(sameFile == false) {
                result += intToCoord(from)[1];
            }
            else if(sameRank == false) {
                result += intToCoord(from)[0];
            } else {
                result += intToCoord(from)[0];
            }
         }
         break;
    }
    case QUEEN: {
        result += "Q";
        queenMoves(side, board, moves, pieces);
        vector<string> squares;
        for(int i = 0; i < moves.size; i++) {
            unsigned long long dest = bitscan(1ULL << ((moves.arr[i] >> 6) & 0x3FULL));
            if(dest == to) {
                squares.push_back(intToCoord(bitscan(1ULL << (moves.arr[i] & 0x3FULL))));
            }
        }
        if(squares.size() > 1) {
            bool sameFile = false;
            bool sameRank = false;
            string source = intToCoord(from);
            char file = source[0];
            char rank = source[1];
            for(int i = 0; i < squares.size(); i++) {
                if(squares.at(i) == source) continue;
                if(squares.at(i)[0] == file) {
                    sameFile = true;
                }
                if(squares.at(i)[1] == rank) {
                    sameRank = true;
                }
            }
            if(sameFile == false ) {
                result += intToCoord(from)[0];
            }
            else if(sameRank == false) {
                result += intToCoord(from)[1];
            }
            else {
                result += intToCoord(from);
            }
         }
         break;
    }
    case KING:
        result += "K";
    }
    if(takes) {
        result += "x";
    }
    result += intToCoord(to);
    if(special == 3) {
        switch (promotion) {
        case KNIGHT - 1:
            result += "=N";
            break;
        case BISHOP - 1:
            result += "=B";
            break;
        case ROOK - 1:
            result += "=R";
            break;
        case QUEEN - 1:
            result += "=Q";
        }
    }

    unsigned long long boardcpy[BITBOARD_SIZE];
    int piececpy[64];
    copy(board, board + BITBOARD_SIZE, boardcpy);
    copy(pieces, pieces + 64, piececpy);
    makeMove(move, boardcpy, piececpy);
    positions.pop();
    if(boardcpy[CHECKMASK] != MAX) {
        fixedVector<unsigned int> moves;
        generateMoves(BLACK - side, boardcpy, moves, piececpy);
        if(moves.size == 0) {
            result += "#";
        } else {
            result += "+";
        }
    }
    return result;
}

inline unsigned long long setBit(unsigned long long number, int n, int x) {
    return number ^ ((-x ^ number) & (1ULL << n));
}

inline bool isRepetition(unsigned long long board[], const historyVector& history, int repetitions) {
    unsigned long long curr = history.arr[history.size - 1];
    int counter = 0;
    for(int i = history.size - 3; i >= 0 && i >= history.size - 1 - ((board[GAME_STATE] & 0xFFF00) >> 8); i -= 2) {
        if(history.arr[i] == curr) counter++;
        if(counter >= repetitions - 1) return true;
    }
    return false;
}

inline bool isDraw(unsigned long long board[], const historyVector& history, int repetitions) {
    return isRepetition(board, history, repetitions) || (((board[GAME_STATE] & 0xFFF00) >> 8) >= 100);
}