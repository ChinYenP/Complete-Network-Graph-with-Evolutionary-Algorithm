#include "binary_addition.hpp"
#include "global.hpp"
#include <vector>
#include <cstdint>
#include <random>
#include <stdexcept>

using namespace std;

vector<bool> Binary_Addition::to_binary(int num) {
    if (num < 0 || num > 255) return (vector<bool>(8,false));
    vector<bool> bits(8);
    for (int i = 7; i >= 0; i--) {
        bits[i] = num & 1;
        num >>= 1;
    }
    return (bits);
}

int Binary_Addition::to_int(const vector<bool>& bits) {
    if (bits.size() != 8) throw invalid_argument("Binary input must be exactly 8 bits");
    int num = 0;
    for (int i = 0; i < 8; i++) {
        num <<= 1;
        num |= bits[i];
    }
    return (num);
}

Binary_Addition::Binary_Addition(uint32_t seed) {
    mt19937 binary_gen(seed);
    uniform_int_distribution<int> number_dist(0, 255);
    a = number_dist(binary_gen);
    b = number_dist(binary_gen);
    c = number_dist(binary_gen);
}

vector<bool> Binary_Addition::next(vector<bool> inputs) {
    int curr_num = -1;
    switch (step) {
        case (0):
            curr_num = a;
            break;
        case (1):
            curr_num = b;
            break;
        case (2):
            curr_num = c;
            break;
    }
    if (step > 0) {
        int input_num = to_int(inputs);
        // if (input_num == curr_sum) {
        //     curr_score += 1.0 / 3.0;
        // }
        if (abs(input_num - curr_sum) <= 25) {
            curr_score += (1.0 / 3.0) * ((25.0 - double(abs(input_num - curr_sum))) / 25.0);
        }
    }
    if (step >= 3) {
        return {};
    }
    curr_sum += curr_num;
    step++;
    return (to_binary(curr_num));
}

double Binary_Addition::score(void) {
    return (curr_score);
}