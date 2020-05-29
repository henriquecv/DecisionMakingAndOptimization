#include "neighborhood.h"
#include "../data-structures/rand.h"

void neighborhood(Problem* problem, int max_neighborhood_time) {
    simulatedAnnealing(problem, max_neighborhood_time);
}

Solution* getNeighbor(Problem* problem) {
    Solution* neighbor;

    std::uniform_int_distribution<int> examsDistribution(0, problem->exams.size() -1);
    std::uniform_int_distribution<int> timeslotsDistribution(0, problem->timeslots -1);

    while ( true ) {
        auto* temp = new Solution(problem->currentSolution);

        // Extract a random exam and a random slot to mutate
        int mutant_exam_id = examsDistribution(generator);
        Exam* mutant_exam = problem->exams[mutant_exam_id];

        int current_slot = temp->examsTimeslots[mutant_exam_id];
        int mutant_slot;
        do{
            mutant_slot = timeslotsDistribution(generator);
        } while( mutant_slot == current_slot );

        /* Check if the exam has no conflict with all the other exams in that timeslot */
        bool feasible = true;
        for (int tmp_exam_id : temp->timeslotsExams[mutant_slot]) {
            if(mutant_exam->hasConflict(tmp_exam_id)) {
                feasible = false;
                break;
            }
        }

        if(feasible){
            temp->moveExam(mutant_exam, mutant_slot);
            return temp;
        }

    }
}

double getCurrentProbability(Problem* problem, Solution* candidate_solution, double T) {
    return exp( -(candidate_solution->getPenalty() - problem->currentSolution->getPenalty()) / T );
}

void simulatedAnnealing(Problem* problem, int max_neighborhood_time) {
    // Initial solution: problem->current_solution
    std::uniform_int_distribution<int> percent_distribution(0, 100);
    double rand_probability, curr_probability;

    /* T_zero should be a number such that: exp( -(F(x^) - F(x~))/T_zero ) = 0.5
     * On internet, someone set T to a high number, something like 1000
     * max t=180/300 seconds. 60%t to multistart, remaining ~108/180s for neighborhood
     */
    double neighborhood_avg_penalty = problem->bestSolution->getPenalty() * 1.1;
    double T_zero = - (neighborhood_avg_penalty - problem->bestSolution->getPenalty()) / log(0.5);

    double T = T_zero;

    /* cooling_rate = [0,1]. Should be near 1, something like 0.99
     * set it to a smaller value will speed up the process but it may skip best solutions
     */
    double cooling_rate = 0.99;

    /* (Plateau) Number of iterations after which temperature T can be changed
     * Suggested to be equal to: 10*neighborhood_size
     */
    int starting_time = time(nullptr);
    int L = 10 * problem->exams.size() * problem->timeslots;

    problem->currentSolution = new Solution(problem->bestSolution); // copy the entire solution if you delete it after
    int time_start_without_improvements = time(nullptr);
    double tmp_best_penalty = problem->bestSolution->getPenalty();

    for(int i = 0; ; ++i) {
        int time_passed_without_improvement = time(nullptr) - time_start_without_improvements;
        if (time_passed_without_improvement > 0.2 * max_neighborhood_time) {

            delete problem->currentSolution;
            problem->currentSolution = new Solution(problem->bestSolution);
        }
        Solution* neighbor = getNeighbor(problem);

        rand_probability = float(percent_distribution(generator)) / 100;
        curr_probability = getCurrentProbability(problem, neighbor, T);

        //T should change only after some steps
        if (i % L == 0) {
            T = T * cooling_rate;
        }

        if (rand_probability < curr_probability) {
            delete problem->currentSolution;
            problem->currentSolution = neighbor;
            problem->handleNewSolution(problem->currentSolution);
        }

        if(problem->bestSolution->getPenalty() < tmp_best_penalty) {
            time_start_without_improvements = time(nullptr);
            tmp_best_penalty = problem->bestSolution->getPenalty();
        }
    }

}