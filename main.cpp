#include "engine.cpp"
TTMap tt;
int main() {
    precomputeMasks();
    setPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", bitboard, piece_board);
    /*string positions[] = {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ", "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ", "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"};
    for(int r = 0; r < 7; r++) {
        setPosition(positions[r], bitboard);
        drawBoard(bitboard);
        for(int i = 1; i <= 7; i++) {
            long long begin = currentMillis();
            cout << "Performance test, i = " << i << " - " << perft(i, bitboard) << endl;
            long long end = currentMillis();
            cout << "Elapsed time: " << end - begin << " ms\n\n";
        }
    }*/

    //3rkb1r/1p3ppp/n1bp1q1n/1N2p1N1/p1B1P2P/4BQ2/PP3PP1/2RR2K1 w k - 0 16
    int engineSide = WHITE;
    while(true) {
        drawBoard(bitboard);
        int side = getSide(bitboard);
        if(side == engineSide) {
            unsigned long long begin = currentMillis();
            unsigned int move = negamax<evaluateNew>(0, 9, -INF, INF, bitboard, piece_board, tt).first;
            cout << "Elapsed time: " << (currentMillis() - begin) << " ms\n";
            printMove(move);
            cout << "\n";
            makeMove(move, bitboard, piece_board);
        } else  {
            string s;
            cin >> s;
            unsigned int move = moveToInt(s, bitboard, piece_board);
            while(move == -1) {
                cout << "Invalid move\n";
                cin >> s;
                move = moveToInt(s, bitboard, piece_board);
            }   
            makeMove(move, bitboard, piece_board);
        }
        if(isDraw(bitboard, positions, 3)) {
            cout << "Draw\n\n\n\n";
            return 0;
        }
    }
}

/*
TEST 1:
    Performance test, i = 1 - 20
    Elapsed time: 0 ms

    Performance test, i = 2 - 400
    Elapsed time: 0 ms

    Performance test, i = 3 - 8902
    Elapsed time: 15 ms

    Performance test, i = 4 - 197281
    Elapsed time: 79 ms

    Performance test, i = 5 - 4865609
    Elapsed time: 1890 ms

    Performance test, i = 6 - 119060324
    Elapsed time: 47203 ms
*/
/*
TEST 2:
    Performance test, i = 1 - 48
    Elapsed time: 0 ms

    Performance test, i = 2 - 2039
    Elapsed time: 0 ms

    Performance test, i = 3 - 97862
    Elapsed time: 31 ms

    Performance test, i = 4 - 4085603
    Elapsed time: 1578 ms

    Performance test, i = 5 - 193690690
    Elapsed time: 71782 ms
*/