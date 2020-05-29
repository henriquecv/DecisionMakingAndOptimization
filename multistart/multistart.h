#ifndef MULTISTART_CPP
#define MULTISTART_CPP

#include <ctime>
#include <vector>
#include <cstdlib>
#include <algorithm>

#include "./GA/Chromosome.h"
#include "../data-structures/Solution.h"
#include "../data-structures/Exam.h"
#include "../data-structures/Solution.h"
#include "../data-structures/Problem.h"

/**
 * Set an initial random population
 * @param problem
 */
void generateInitialPopulation(Problem* problem);

//Sort the population according to the decreasing chromosomes' fitness
void sortPopulation(Problem* problem);

//Create a new population using the current population
void evolvePopulation(Problem* problem);

/** Compute the suggested population size starting from the problem size (number of exams)
 *
 * @param problem
 * @return suggested population size
 */
int computePopulationSize(Problem* problem);

/**
 * Execute a multi-start algorithm on the given problem
 * @param problem
 */
void multistart(Problem* problem, int maxTime);

#endif //MULTISTART_CPP
