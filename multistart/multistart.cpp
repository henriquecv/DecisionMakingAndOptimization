#include <cmath>
#include "multistart.h"
#include "../data-structures/rand.h"

int populationSize;

// Population vector
std::vector<Chromosome*> chromosomes;

void generateInitialPopulation(Problem* problem) {

    /* Statistics on generated solutions */ //TODO: remove for production for better performances
    double tmp_penalty;
    double min_penalty = std::numeric_limits<double>::max();
    double max_penalty = std::numeric_limits<double>::min();
    double sum_penalty = 0;

    for (int i = 0; i < populationSize; i++) {

        Chromosome* c = new Chromosome(problem, true);
        chromosomes.push_back(c);

        tmp_penalty = c->solution->getPenalty();
        min_penalty = (tmp_penalty < min_penalty) ? tmp_penalty : min_penalty;
        max_penalty = (tmp_penalty > max_penalty) ? tmp_penalty : max_penalty;
        sum_penalty += tmp_penalty;

        problem->handleNewSolution(c->solution);

    }

    printf("Random starting solution statistics:\n");
    printf("- Max penalty:  %f\n", max_penalty);
    printf("- Min penalty:  %f\n", min_penalty);
    printf("- Mean penalty: %f\n", sum_penalty / populationSize);

}

void sortPopulation(Problem* problem) {

    // Define a lambda to sort chromosomes
    std::sort(chromosomes.begin(), chromosomes.end(), [](Chromosome *one, Chromosome *two) {
        return one->solution->getPenalty() < two->solution->getPenalty();
    });

    // Evaluate new solution
    problem->handleNewSolution(chromosomes[0]->solution);

}

void evolvePopulation(Problem* problem) {

    // Sort the population in order to have easy access to the best chromosomes
    sortPopulation(problem);

    // Initialize some variables
    std::vector<Chromosome*> offspring;
    int p = int(populationSize * 0.1);

    // There are different ways to generate a new population:
    // At the beginning you should use crossover a lot,
    // then reaching the end you should use more mutation

    // In this case I use only crossover, it works fine but when the sentence is almost complete(one or two characters are different)
    // it slows down and it takes lot of generations to find the first complete solution.

    /*  Given a population: (kind of)
            a) 20% are the top Chromosomes, these will not be changed
            b) 30% are generated with Crossover between random Chromosomes
            c) 30% are generated with Mutation of Top Chromosomes
            d) 20% are generated with Inversion of Top Chromosomes
    */
    // TODO: add conditions for population size < 4
    int b_start = p * 7;
    int b_stop = populationSize - 1;

    int c_start = p * 4;
    int c_stop = p * 7;

    int d_start = p * 2;
    int d_stop = p * 4;

    // (b) Crossover between random Chromosomes: It selects randomly two parents to do the crossover operation.

    for (int j = b_start; j < b_stop ; j = j + 2) {
        // Generate offspring

        std::uniform_int_distribution<int> population_distribution(0, populationSize - 1);

        // Random parents
        int parent_1 = population_distribution(generator);
        int parent_2;
        do{
            parent_2 = population_distribution(generator);
        } while(parent_1 == parent_2);

        Chromosome* newchild1 = Chromosome::crossover(problem, chromosomes[parent_1], chromosomes[parent_2]);
        Chromosome* newchild2 = Chromosome::crossover(problem, chromosomes[parent_2], chromosomes[parent_1]);

        // Add the new children in the population if they are better then the chromosome they will replace
        delete chromosomes[j];
        chromosomes[j] = newchild1;

        delete chromosomes[j + 1];
        chromosomes[j + 1] = newchild2;
    }

    // (c) Mutation on Top Chromosomes: It tries to move a random exam from one timeslot to another.

    for (int j = c_start; j < c_stop; j++) {
        delete chromosomes[j];
        chromosomes[j] = chromosomes[j - p*4]->mutation(problem);
    }

    // (d) Inversion on Top Chromosomes: It moves all exams from one random timeslot T1 to another timeslot T2 and all exams from T2 to T1.
    for (int j = d_start; j < d_stop; j++) {
        delete chromosomes[j];
        chromosomes[j] = chromosomes[j - p*2]->timeslot_inversion(problem);
    }

}

int computePopulationSize(Problem* problem) {

    // Source: https://www.researchgate.net/post/What_is_the_optimal_recommended_population_size_for_differential_evolution2

    // Get problem size
    int problemSize = problem->exams.size();
    return std::max(int(problemSize * 0.1), 10);

}

void multistart(Problem* problem, int maxTime) {

    int startingTime = time(nullptr);
    int stoppingTime = startingTime + maxTime;

    // Compute population size accordingly to problem size
    populationSize = computePopulationSize(problem);

    // Generate initial population
    generateInitialPopulation(problem);

    int time_start_generations_without_improvements = time(nullptr);
    double tmp_best_penalty = problem->bestSolution->getPenalty();

    while(time(nullptr) < stoppingTime) {
        if(problem->bestSolution->getPenalty() < tmp_best_penalty) {
            time_start_generations_without_improvements = time(nullptr);
            tmp_best_penalty = problem->bestSolution->getPenalty();
        }

        int time_run = time(nullptr) - problem->start_time;
        int time_passed_without_improvement = time(nullptr) - time_start_generations_without_improvements;
        if (time_run > problem->max_time * 0.5 && time_passed_without_improvement > 0.1 * maxTime) {
            return;
        }

        evolvePopulation(problem);
    }

}