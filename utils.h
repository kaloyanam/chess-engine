#include "constants.h"
using namespace std;

template<typename T>
struct fixedVector {
    T arr[256];
    int size = 0;
    void push_back(T i) {arr[size++] = i;}
    void clear() {size = 0;}
    vector<T> toVector() {return vector<T>(arr, arr + size);}
};

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


inline unsigned long long setBit(unsigned long long number, int n, int x) {
    return number ^ ((-x ^ number) & (1ULL << n));
}
