#pragma once

#include <vector>
#include <string>

using namespace std;

string random_ID(void);

struct node {
    double value;
    double threshold;
    double decay_rate; //Between 0.0 to 1.0, skew towards 1.0
    unsigned int firing_timeout_num_tick;
    unsigned int firing_timeout_remaining_tick;
};

//First node is always forever input of 1.
class AI {
    private:
        //edges[from][to]
        string ID;
        unsigned int input_size;
        unsigned int output_size;
        vector<vector<double>> edges; //Weightage, all edges from the same node will add up to 1.0
        vector<node> nodes;
        vector<double> output_energy_used;
        vector<double> scale_invariant_random(unsigned int n);
        double addition_nudge(double value);
        double multiplication_nudge(double value);
        unsigned int max_num_variable_mutate(void) const;
    public:
        AI(unsigned int num_hidden_nodes, unsigned int in_len, unsigned int out_len);
        AI(const AI& other);
        AI& operator=(const AI& other);
        vector<bool> iteration(vector<bool> in, bool output_query = false);
        void mutation(void);
        string get_ID(void);
        unsigned int num_nodes(void) const;
};