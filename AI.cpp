#include "AI.hpp"
#include "global.hpp"
#include <vector>
#include <random>
#include <chrono>
#include <string>
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

vector<double> AI::scale_invariant_random(unsigned int n) {
    //Simplex
    vector<double> nums;
    for (unsigned int i = 0; i < n - 1; i++) {
        nums.push_back(uniform_real_distribution<double>(0.0, 1.0)(generator));
    }
    sort(nums.begin(), nums.end());
    vector<double> diffs;
    double prev = 0.0;
    for (unsigned int i = 0; i < n - 1; i++) {
        diffs.push_back(nums[i] - prev);
        prev = nums[i];
    }
    diffs.push_back(1.0 - prev);
    return (diffs);
}

double AI::addition_nudge(double value) {
    uniform_real_distribution<double> nudge_dist(-0.5, 0.5);
    return (value + nudge_dist(generator));
}

double AI::multiplication_nudge(double value) {
    uniform_real_distribution<double> nudge_dist(0.333, 3.0);
    return (value * nudge_dist(generator));
}

string random_ID(void) {
    const string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    uniform_int_distribution<int> char_dist(0, characters.size() - 1);
    string id = "";
    for (size_t i = 0; i < 6; i++) {
        id += characters[char_dist(generator)];
    }
    return (id);
}

unsigned int AI::max_num_variable_mutate(void) const {
    const unsigned int num_hidden_nodes = num_nodes() - input_size - 1 - output_size;
    return (num_hidden_nodes * num_hidden_nodes * 0.01);
}

AI::AI(unsigned int num_hidden_nodes, unsigned int in_len, unsigned int out_len) {
    input_size = in_len;
    output_size = out_len;
    unsigned int n = num_hidden_nodes + in_len + 1 + out_len;
    ID = random_ID();
    for (unsigned int i = 0; i < output_size; i++) {
        output_energy_used.push_back(unbounded_random());
    }
    edges = vector<vector<double>>(n, vector<double>(n, 0.0));
    nodes = vector<node>(n, {
        .value = 0.0,
        .threshold = 0.0,
        .decay_rate = 1.0,
        .firing_timeout_num_tick = 0,
        .firing_timeout_remaining_tick = 0
    });
    for (unsigned int i = 0; i < n; i++) {
        nodes[i].threshold = unbounded_random();
        nodes[i].decay_rate = bounded_random(0.0, 1.0);
        nodes[i].firing_timeout_num_tick = floor(unbounded_random());
        vector<double> edge_weightages = scale_invariant_random(n);
        for (unsigned int j = 0; j < n; j++) {
            edges[i][j] = edge_weightages[j];
        }
    }
    //Output nodes threshold correction
    for (unsigned int i = 0; i < output_size; i++) {
        if (nodes[n - output_size + i].threshold < output_energy_used[i]) {
            nodes[n - output_size + i].threshold = output_energy_used[i];
        }
    }
}

AI::AI(const AI& other) {
    ID = other.ID;
    input_size = other.input_size;
    output_size = other.output_size;
    edges = other.edges;
    nodes = other.nodes;
    output_energy_used = other.output_energy_used;
    for (unsigned int i = 0; i < other.num_nodes(); i++) {
        nodes[i].value = 0.0;
        nodes[i].firing_timeout_remaining_tick = 0;
    }
}

AI& AI::operator=(const AI& other) {
    if (this == &other) return *this;
    ID = other.ID;
    input_size = other.input_size;
    output_size = other.output_size;
    edges = other.edges;
    nodes = other.nodes;
    output_energy_used = other.output_energy_used;
    for (unsigned int i = 0; i < other.num_nodes(); i++) {
        nodes[i].value = 0.0;
        nodes[i].firing_timeout_remaining_tick = 0;
    }
    return (*this);
}

vector<bool> AI::iteration(vector<bool> in, bool output_query) {
    if (in.size() != input_size) {
        cout << "Invalid input size!" << endl;
        return (vector<bool>(0,false));
    }

    //Input and output
    nodes[0].value += 1.0;
    for (unsigned int i = 0; i < in.size(); i++) {
        nodes[i+1].value += double(in[i]);
    }
    vector<bool> output(output_size, false);
    if (output_query) {
        for (unsigned int i = 0; i < output_size; i++) {
            if (nodes[num_nodes() - output_size + i].value >= nodes[num_nodes() - output_size + i].threshold) {
                output[i] = true;
                nodes[num_nodes() - output_size + i].value -= output_energy_used[i];
            }
        }
    }
    
    //Let nodes send values
    vector<node> new_nodes = nodes;
    for (unsigned int i = 0; i < num_nodes(); i++) {
        new_nodes[i].value = 0.0;
        if (new_nodes[i].firing_timeout_remaining_tick > 0) {
            new_nodes[i].firing_timeout_remaining_tick--;
        }
    }
    for (unsigned int i = 0; i < num_nodes(); i++) {
        if (new_nodes[i].firing_timeout_remaining_tick > 0 || nodes[i].value < nodes[i].threshold) {
            new_nodes[i].value += nodes[i].value;
            continue;
        }
        new_nodes[i].firing_timeout_remaining_tick = new_nodes[i].firing_timeout_num_tick;
        for (unsigned int j = 0; j < num_nodes(); j++) {
            new_nodes[j].value += nodes[i].value * edges[i][j];
        }
    }
    for (unsigned int i = 0; i < num_nodes(); i++) {
        new_nodes[i].value *= new_nodes[i].decay_rate;
    }
    nodes = new_nodes;

    return (output);
}

void AI::mutation(void) {
    uniform_int_distribution<int> index_dist(0, num_nodes() - 1);
    uniform_int_distribution<int> num_mutation_dist(1, max(1u, max_num_variable_mutate()));
    unsigned int num_mutation = num_mutation_dist(generator);
    for (unsigned int i = 0; i < num_mutation; i++) {
        double random_num = uniform_real_distribution<double>(0.0, 1.0)(generator);
        if (random_num < 0.35) {
            //Edge
            unsigned int from_index = index_dist(generator);
            unsigned int to_index = index_dist(generator);
            double new_weight = multiplication_nudge(edges[from_index][to_index]);
            double new_weight_sum = 1.0 + (new_weight - edges[from_index][to_index]);
            edges[from_index][to_index] = new_weight;
            for (unsigned int j = 0; j < num_nodes(); j++) {
                edges[from_index][j] /= new_weight_sum;
            }
        } else if (random_num < 0.7) {
            //Node
            unsigned int target_index = index_dist(generator);
            random_num = uniform_real_distribution<double>(0.0, 1.0)(generator);
            if (random_num < 0.33333) {
                //Threshold
                nodes[target_index].threshold = multiplication_nudge(nodes[target_index].threshold);
                if (target_index >= num_nodes() - output_size) {
                    //Output node, adjust threshold
                    if (nodes[target_index].threshold < output_energy_used[target_index - (num_nodes() - output_size)]) {
                        nodes[target_index].threshold = output_energy_used[target_index - (num_nodes() - output_size)];
                    }
                }
            } else if (random_num < 0.66666) {
                //Decay rate
                nodes[target_index].decay_rate = addition_nudge(nodes[target_index].decay_rate);
                if (nodes[target_index].decay_rate < 0.0) {
                    nodes[target_index].decay_rate = 0.0;
                } else if (nodes[target_index].decay_rate > 1.0) {
                    nodes[target_index].decay_rate = 1.0;
                }
            } else {
                //firing timeout
                nodes[target_index].firing_timeout_num_tick = max(1, 1 + int(multiplication_nudge(nodes[target_index].firing_timeout_num_tick)));
            }
        } else {
            //Output energy
            uniform_int_distribution<int> out_index_dist(0, output_size - 1);
            int target_index = out_index_dist(generator);
            output_energy_used[target_index] = addition_nudge(output_energy_used[target_index]);
            if (output_energy_used[target_index] < 0.0) {
                output_energy_used[target_index] = 0.0;
            } else if (output_energy_used[target_index] > 1.0) {
                output_energy_used[target_index] = 1.0;
            }
        }
    }
}

string AI::get_ID(void) {
    return (ID);
}

unsigned int AI::num_nodes(void) const {
    return (nodes.size());
}