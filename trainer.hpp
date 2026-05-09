#pragma once

#include "AI.hpp"
#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <concepts>

using namespace std;

constexpr unsigned int NUM_AI = 100;
static_assert(NUM_AI >= 10, "Too less AI.");
constexpr unsigned int NUM_AI_PASS = 30;
constexpr unsigned int NUM_TRAINING_PER_EVO = 100;
constexpr unsigned int NUM_ITERATION_PER_NEXT = 5;
constexpr unsigned int NUM_SEEDS_PER_MUTATE = 5;
constexpr unsigned int NUM_COPIES_PER_MUTATE = 30;
constexpr unsigned int NUM_TEST_SEEDS = 50;

template <typename T>
concept fitness_class = constructible_from<T, uint32_t> &&
requires(T obj, vector<bool> input) {
    { obj.next(input) } -> same_as<vector<bool>>; //Return empty vector to signal end of simulation
    { obj.score() } -> same_as<double>;
};

struct candidate {
    AI ai;
    double score;
    double best_score;
    candidate(unsigned int hidden_nodes, unsigned int in_len, unsigned int out_len):
        ai(hidden_nodes, in_len, out_len),
        score(0.0),
        best_score(0.0)
        {};
};

template <fitness_class F>
class Trainer {
    private:
        unsigned int input_length;
        unsigned int output_length;
        vector<candidate> candidates;
        vector<uint32_t> training_seeds;
        ofstream logfile;
        double best_score = 0.0;
        unsigned int evolution_num = 0;
        double simulation(AI ai, uint32_t seed);
        void output_evolution_scores(void);
    public:
        Trainer(unsigned int in_len, unsigned int out_len, string filename);
        void one_evolution(void);
        AI best_ai(void);
        void testing(void);
};

#include "trainer.tpp"