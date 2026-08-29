#include "engine.cpp"
#include <sstream>
using namespace std;
unsigned long long board[BITBOARD_SIZE];
int pieces[64];
TTMap tt;
const int overhead = 200;
inline void handshake(string name, string author) {
    cout << "id name " + name << endl;
    cout << "id author " + author << endl;
    cout << "uciok" << endl;
}

inline void readyok() {
    cout << "readyok" << endl;
}

inline void newgame() {
    fill(board, board + BITBOARD_SIZE, 0);
    fill(pieces, pieces + 64, -1);
    tt.clear();
    positions.clear();
}

inline bool isValidMove(unsigned int move, unsigned long long board[], int pieces[]) {
    bool isValid = false;
    fixedVector<unsigned int> m;
    generateMoves(getSide(board), board, m, pieces);
    for(int i = 0; i < m.size; i++) {
        if(m.arr[i] == move) {
            isValid = true;
            break;
        }
    }
    return isValid;
}

inline void setupFen(string fen, vector<string> moves) {
    setPosition(fen, board, pieces);
    for(int i = 0; i < moves.size(); i++) {
        unsigned int move = uciToMove(moves[i], board, pieces);
        if(isValidMove(move, board, pieces))
            makeMove(move, board, pieces);
    }
}

inline void setupStart(vector<string> moves) {
    setPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", board, pieces);
    for(int i = 0; i < moves.size(); i++) {
        unsigned int move = uciToMove(moves[i], board, pieces);
        if(isValidMove(move, board, pieces))
            makeMove(move, board, pieces);
    }
}

inline void makeMove(int wtime, int btime, int winc, int binc) {
    int side = getSide(board);
    int time = (side == WHITE ? wtime : btime);
    int inc = (side == WHITE ? winc : binc);
    int budget = max(50, min(time / 30 + inc, time - overhead));
    unsigned int bestMove = incrementalSearch<evaluateNew>(budget, board, pieces, tt).bestMove;
    cout << "bestmove " << moveToUci(bestMove) << endl;
}

inline void makeMove(int time) {
    unsigned int bestMove = incrementalSearch<evaluateNew>(max(50, time - overhead), board, pieces, tt).bestMove;
    cout << "bestmove " << moveToUci(bestMove) << endl;
}

int main() {
    precomputeMasks();
    string line;
    while(getline(cin, line)) {
        istringstream iss(line);
        string command;
        iss >> command;
        if(command == "uci") {
            handshake("chess-engine 1.0", "Kaloyan Marinov");
        }
        else if(command == "isready") {
            readyok();
        }
        else if(command == "ucinewgame") {
            newgame();
        }
        else if(command == "position") {
            string origin;
            iss >> origin;
            if(origin == "startpos") {
                string buff;
                iss >> buff;
                string move;
                vector<string> moves;
                while(iss >> move) {
                    moves.push_back(move);
                }
                setupStart(moves);
            }
            else if(origin == "fen") {
                string buff;
                string fen;
                while((iss >> buff) && buff != "moves") {
                    fen += buff + " ";
                }
                string move;
                vector<string> moves;
                while(iss >> move) {
                    moves.push_back(move);
                }
                setupFen(fen, moves);
            }
        }
        else if(command == "go") {
            int wtime = 0, btime = 0, winc = 0, binc = 0, movetime = -1;
            string attr;
            while(iss >> attr) {
                if(attr == "wtime") {
                    iss >> wtime;
                }
                else if(attr == "btime") {
                    iss >> btime;
                }
                else if(attr == "winc") {
                    iss >> winc;
                }
                else if(attr == "binc") {
                    iss >> binc;
                }
                else if(attr == "movetime") {
                    iss >> movetime;
                }
            }
            if(movetime >= 0) 
                makeMove(movetime);
            else
                makeMove(wtime, btime, winc, binc);
        }
        else if(command == "quit") {
            return 0;
        }
        else {
            continue;
        }
    }
}