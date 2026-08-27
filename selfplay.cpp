#include "engine.cpp"
using namespace std;
TTMap ttOld;
TTMap ttNew;
int main() {
    precomputeMasks();
    setPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", bitboard, piece_board);
    for(int i = 1;true; i++) {
        cout << i << ". ";
        unsigned int move = negamax<evaluateOld>(0, 7, -INF, INF, bitboard, piece_board, ttOld).first;
        if(move == 0) break;
        //printMove(move);
        cout << intToMove(move, bitboard, piece_board) << " ";
        makeMove(move, bitboard, piece_board);
        move = negamax<evaluateNew>(0, 7, -INF, INF, bitboard, piece_board, ttNew).first;
        if(move == 0) break;
        //printMove(move);
        cout << intToMove(move, bitboard, piece_board) << endl;
        makeMove(move, bitboard, piece_board);
    }
}