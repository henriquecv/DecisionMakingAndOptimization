#ifndef NEIGHBORHOOD_CPP
#define NEIGHBORHOOD_CPP

#include "../data-structures/Solution.h"
#include "../data-structures/Problem.h"
#include <cmath> //exp
#include <random>
#include <ctime>

/**
 * Starting from a given solution, get another solution in the neighborhood
 *
 * @param current_solution the solution where to start from
 * @return the neighbor solution
 */
Solution* getNeighbor(Problem* problem, Solution* current_solution);

/*
 * Execute Simulated Annealing algorithm on the given problem
 * @param problem
 */
void simulatedAnnealing(Problem* problem, int max_neighborhood_time);

/**
 * Execute a neighborhood algorithm on the given problem
 * @param problem
 */
void neighborhood(Problem* problem, int max_neighborhood_time);

#endif //NEIGHBORHOOD_CPP
