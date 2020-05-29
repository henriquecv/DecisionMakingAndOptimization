#include <vector>

#include "../../data-structures/Exam.h"
#include "../../data-structures/Solution.h"
#include "../../data-structures/Problem.h"

class Chromosome {

public:

    /** Chromosome object with a feasible solution initialized
     * @param problem
     */
    explicit Chromosome(Problem* problem, bool improved_solution = false);

    /**
     * Pre-initialized chromosome
     * @param problem
     * @param initializingSolution
     * @param mutationRate
     */
    Chromosome(Problem* problem, int *initializingSolution);

    // Destructor
    ~Chromosome();

    // Genetic operators

    /**
     * Generates a copy of current genes
     * @return array of dimension "n_exams", each containing the assigned timeslot
     */
    int *getGenes();

    /**
     * Mutate one random gene, assigning to it one random timeslot, in order to diversify the search and avoid getting stuch in a local minimum
     * WARNING: FEASIBILITY NOT GUARANTEED!
     */
    Chromosome* mutation(Problem* problem);
    void mutation();
    static Chromosome* crossover(Problem* problem, Chromosome *firstParent, Chromosome *secondParent);

    /**
     * Swap genes
     */
    Chromosome *inversion(Problem* problem);
    Chromosome *timeslot_inversion(Problem* problem);

    double getFitness();

    // Solution object
    Solution *solution;

    // Mutation rate
    float mutationRate;

};
