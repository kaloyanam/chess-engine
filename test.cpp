#include "engine.cpp"
#include <cassert>
using namespace std;
inline long long testPosition(int depth, string fen, long long expected) {
    long long elapsedBegin = currentMillis();
    long long result = perft(depth, fen);
    if(expected != result) {
        cerr << "\nAssertation failed.\nExpected value: " << expected << "\nReceived value: " << result << "\n";
        abort();
    }
    long long elapsedEnd = currentMillis();
    return elapsedEnd - elapsedBegin;
}

int main() {
    cout << "Precomputing masks...\n";
    precomputeMasks();
    cout << "Done\n";

    //Position 1
    cout << "Initial position working correctly, elapsed time: " << testPosition(6, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 119060324) << " ms\n";

    //Position 2
    cout << "Position 2 working correctly, elapsed time: " << testPosition(5, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 193690690) << " ms\n";

    //Position 3
    cout << "Position 3 working correctly, elapsed time: " << testPosition(6, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1 ", 11030083) << " ms\n";

    //Position 4
    cout << "Position 4 working correctly, elapsed time: " << testPosition(5, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 15833292) << " ms\n";

    //Position 5
    cout << "Position 5 working correctly, elapsed time: " << testPosition(5, "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 89941194) << " ms\n";
    
    //Position 6
    cout << "Position 6 working correctly, elapsed time: " << testPosition(5, "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 164075551) << " ms\n";

}
/*
Initial position working correctly, elapsed time: 31805 ms
Position 2 working correctly, elapsed time: 49861 ms
Position 3 working correctly, elapsed time: 2782 ms
Position 4 working correctly, elapsed time: 4586 ms
Position 5 working correctly, elapsed time: 24125 ms
Position 6 working correctly, elapsed time: 42057 ms
_____________ After fixed vector
Initial position working correctly, elapsed time: 22531 ms
Position 2 working correctly, elapsed time: 35730 ms
Position 3 working correctly, elapsed time: 1654 ms
Position 4 working correctly, elapsed time: 3052 ms
Position 5 working correctly, elapsed time: 17003 ms
Position 6 working correctly, elapsed time: 31249 ms
_____________ After correct compilation flags g++ $fileName -w -O3 -march=native -flto -o
Initial position working correctly, elapsed time: 2102 ms
Position 2 working correctly, elapsed time: 3480 ms
Position 3 working correctly, elapsed time: 144 ms
Position 4 working correctly, elapsed time: 323 ms
Position 5 working correctly, elapsed time: 1636 ms
Position 6 working correctly, elapsed time: 3147 ms
_____________ After perft optimization
Initial position working correctly, elapsed time: 262 ms
Position 2 working correctly, elapsed time: 394 ms
Position 3 working correctly, elapsed time: 25 ms
Position 4 working correctly, elapsed time: 35 ms
Position 5 working correctly, elapsed time: 181 ms
Position 6 working correctly, elapsed time: 337 ms
*/