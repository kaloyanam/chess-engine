#include "engine.cpp"
using namespace std;
TTMap ttOld;
TTMap ttNew;
int main() {
    int timePerMove = 5000;
    precomputeMasks();
    setPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", bitboard, piece_board);
    for(int i = 1;true; i++) {
        cout << i << ". ";
        unsigned int move = incrementalSearch<evaluateOld>(timePerMove, bitboard, piece_board, ttOld).bestMove;
        if(move == 0) break;
        //printMove(move);
        cout << intToMove(move, bitboard, piece_board) << " ";
        makeMove(move, bitboard, piece_board);
        move = incrementalSearch<evaluateNew>(timePerMove, bitboard, piece_board, ttNew).bestMove;
        if(move == 0) break;
        //printMove(move);
        cout << intToMove(move, bitboard, piece_board) << endl;
        makeMove(move, bitboard, piece_board);
    }
}