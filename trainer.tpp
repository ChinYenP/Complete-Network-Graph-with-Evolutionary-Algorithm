#include "global.hpp"
#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>
#include <atomic>
#include <cmath>
#include <string>
#include <fstream>
#include <concepts>
#include <stdexcept>

using namespace std;

bool cmp_AI_func(const candidate &a, const candidate &b) {
    long long a_score_rounded = round(a.score / 0.00001);
    long long b_score_rounded = round(b.score / 0.00001);
    if (a_score_rounded == b_score_rounded) {
        return (a.best_score > b.best_score);
    }
    return (a_score_rounded > b_score_rounded);
}

template <fitness_class F>
double Trainer<F>::simulation(AI ai, uint32_t seed) {
    ai = AI(ai);
    F fitness_obj(seed);
    vector<bool> AI_output(output_length, false);
    while (true) {
        const vector<bool> AI_input = fitness_obj.next(AI_output);
        if (AI_input.size() == 0) break; //End simulation
        if (AI_input.size() != input_length) {
            throw invalid_argument("Your fitness class next()'s return vector size is not same as input_length.");
        }
        for (unsigned int i = 0; i < NUM_ITERATION_PER_NEXT - 1; i++) {
            ai.iteration(AI_input);
        }
        AI_output = ai.iteration(AI_input, true);
    }
    return (fitness_obj.score());
}

template <fitness_class F>
void Trainer<F>::output_evolution_scores(void) {
    cout << "Best score: " << best_score << endl;
    cout << "Top 1 (" << candidates[0].ai.get_ID() << ") score: " << candidates[0].score << " (" << candidates[0].best_score << ")" << endl;
    cout << "Top 2 (" << candidates[1].ai.get_ID() << ") score: " << candidates[1].score << " (" << candidates[1].best_score << ")" << endl;
    cout << "Top 3 (" << candidates[2].ai.get_ID() << ") score: " << candidates[2].score << " (" << candidates[2].best_score << ")" << endl;
    cout << "Top 4 (" << candidates[3].ai.get_ID() << ") score: " << candidates[3].score << " (" << candidates[3].best_score << ")" << endl;
    cout << "Top 5 (" << candidates[4].ai.get_ID() << ") score: " << candidates[4].score << " (" << candidates[4].best_score << ")" << endl;
    cout << "Top " << NUM_AI_PASS << " score: (" << candidates[NUM_AI_PASS - 1].ai.get_ID() << ") score: " << candidates[NUM_AI_PASS - 1].score  << " (" << candidates[NUM_AI_PASS - 1].best_score << ")" << endl;
    logfile << "Best score: " << best_score << endl;
    logfile << "Top 1 (" << candidates[0].ai.get_ID() << ") score: " << candidates[0].score << " (" << candidates[0].best_score << ")" << endl;
    logfile << "Top 2 (" << candidates[1].ai.get_ID() << ") score: " << candidates[1].score << " (" << candidates[1].best_score << ")" << endl;
    logfile << "Top 3 (" << candidates[2].ai.get_ID() << ") score: " << candidates[2].score << " (" << candidates[2].best_score << ")" << endl;
    logfile << "Top 4 (" << candidates[3].ai.get_ID() << ") score: " << candidates[3].score << " (" << candidates[3].best_score << ")" << endl;
    logfile << "Top 5 (" << candidates[4].ai.get_ID() << ") score: " << candidates[4].score << " (" << candidates[4].best_score << ")" << endl;
    logfile << "Top " << NUM_AI_PASS << " score: (" << candidates[NUM_AI_PASS - 1].ai.get_ID() << ") score: " << candidates[NUM_AI_PASS - 1].score  << " (" << candidates[NUM_AI_PASS - 1].best_score << ")" << endl;
}

template <fitness_class F>
Trainer<F>::Trainer(unsigned int in_len, unsigned int out_len, string filename): input_length(in_len), output_length(out_len), logfile(filename) {
    candidates.reserve(NUM_AI);
    for (unsigned int i = 0; i < NUM_AI; i++) {
        candidate dummy(unbounded_random(25.0), in_len, out_len);
        candidates.emplace_back(dummy);
    }
    for (unsigned int i = 0; i < NUM_TRAINING_PER_EVO; i++) {
        training_seeds.push_back(uniform_int_distribution<uint32_t>(0,numeric_limits<uint32_t>::max())(generator));
    }
}

template <fitness_class F>
void Trainer<F>::one_evolution(void) {
    cout << "\nEvolution " << evolution_num << endl;
    logfile << "\nEvolution " << evolution_num << endl;
    uniform_int_distribution<int> train_seed_index_dist(0, NUM_TRAINING_PER_EVO - 1);
    vector<uint32_t> curr_seeds;
    for (unsigned int i = 0; i < NUM_SEEDS_PER_MUTATE; i++) {
        curr_seeds.push_back(training_seeds[train_seed_index_dist(generator)]);
    }

    for (unsigned int candidates_index = 0; candidates_index < candidates.size(); candidates_index++) {
        cout << "AI " << candidates_index << endl;
        logfile << "AI " << candidates_index << endl;
        //Let original AI play first
        candidates[candidates_index].score = 0.0;
        for (unsigned int seed_index = 0; seed_index < NUM_SEEDS_PER_MUTATE; seed_index++) {
            candidates[candidates_index].score += simulation(candidates[candidates_index].ai, curr_seeds[seed_index]);
        }
        candidates[candidates_index].score /= NUM_SEEDS_PER_MUTATE;
        if (candidates[candidates_index].score > candidates[candidates_index].best_score) {
            candidates[candidates_index].best_score = candidates[candidates_index].score;
        }
        if (candidates[candidates_index].score > best_score) {
            best_score = candidates[candidates_index].score;
        }
        
        //Now do mutation repetition
        bool continue_mutation = true;
        while (continue_mutation) {
            cout << "Mutate..." << endl;
            logfile << "Mutate..." << endl;
            continue_mutation = false;
            double curr_best_score = candidates[candidates_index].score;
            AI curr_best_AI(candidates[candidates_index].ai);
            for (unsigned int j = 0; j < NUM_COPIES_PER_MUTATE; j++) {
                double curr_score = 0.0;
                AI curr_AI(curr_best_AI);
                curr_AI.mutation();
                for (unsigned int seed_index = 0; seed_index < NUM_SEEDS_PER_MUTATE; seed_index++) {
                    curr_score += simulation(curr_AI, curr_seeds[seed_index]);
                }
                curr_score /= NUM_SEEDS_PER_MUTATE;
                if (curr_score > curr_best_score) {
                    curr_best_score = curr_score;
                    curr_best_AI = AI(curr_AI);
                    continue_mutation = true;
                }
                if (curr_score > best_score) {
                    best_score = curr_score;
                }
            }
            candidates[candidates_index].ai = AI(curr_best_AI);
            candidates[candidates_index].score = curr_best_score;
            if (curr_best_score > candidates[candidates_index].best_score) {
                candidates[candidates_index].best_score = curr_best_score;
            }
        }
    }

    shuffle(candidates.begin(), candidates.end(), generator);
    sort(candidates.begin(), candidates.end(), cmp_AI_func);
    
    output_evolution_scores();
    
    for (unsigned int passed_candidates_index = 0; passed_candidates_index < NUM_AI_PASS; passed_candidates_index++) {
        candidates[passed_candidates_index].ai = AI(candidates[passed_candidates_index].ai);
    }
    for (unsigned int failed_candidates_index = NUM_AI_PASS; failed_candidates_index < NUM_AI; failed_candidates_index++) {
        candidates[failed_candidates_index].ai = AI(unbounded_random(25.0), input_length, output_length);
    }
    for (unsigned int candidates_index = 0; candidates_index < NUM_AI; candidates_index++) {
        candidates[candidates_index].score = 0.0;
    }
    evolution_num++;
}

template <fitness_class F>
AI Trainer<F>::best_ai(void) {
    return (candidates[0].ai);
}

template <fitness_class F>
void Trainer<F>::testing(void) {
    cout << "\nStart Test:" << endl;
    logfile << "\nStart Test:" << endl;
    AI top_AI = best_ai();
    vector<uint32_t> test_seeds;
    for (unsigned int i = 0; i < NUM_TEST_SEEDS; i++) {
        test_seeds.push_back(uniform_int_distribution<uint32_t>(0,numeric_limits<uint32_t>::max())(generator));
    }
    for (unsigned int seed_index = 0; seed_index < NUM_TEST_SEEDS; seed_index++) {
        double score = simulation(top_AI, test_seeds[seed_index]);
        cout << "Test " << seed_index << ": " << score << endl;
        logfile << "Test " << seed_index << ": " << score << endl;
    }
}