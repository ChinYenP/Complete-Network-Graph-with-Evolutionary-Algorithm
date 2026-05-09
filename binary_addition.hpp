#pragma once

#include "global.hpp"
#include <vector>
#include <cstdint>

using namespace std;

class Binary_Addition {
    private:
        int a;
        int b;
        int c;
        int curr_sum = 0;
        int step = 0;
        double curr_score = 0.0;
        vector<bool> to_binary(int num);
        int to_int(const std::vector<bool>& bits);
    public:
        Binary_Addition(uint32_t seed);
        vector<bool> next(vector<bool> inputs);
        double score(void);
};