#pragma once
#include <random>

extern std::mt19937 generator;

double bounded_random(double minimum, double maximum);
double unbounded_random(double mean = 1.0);