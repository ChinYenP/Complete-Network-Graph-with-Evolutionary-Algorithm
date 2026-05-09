#include "global.hpp"
#include <random>
#include <chrono>

using namespace std;

double bounded_random(double minimum, double maximum) {
    uniform_real_distribution<double> bounded_dist(minimum, maximum);
    return (bounded_dist(generator));
}

double unbounded_random(double mean) {
    exponential_distribution<double> exp_dist(1.0 / mean);
    return exp_dist(generator);
}

random_device rd;
mt19937 generator(rd());