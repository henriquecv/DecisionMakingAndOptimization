#include <algorithm>
#include <iterator>
#include <vector>

#include "Chromosome.h"
#include "../../data-structures/Solution.h"
#include "../../data-structures/Problem.h"
#include "../../data-structures/rand.h"

float mutRate = 0.8;

Chromosome::Chromosome(Problem* problem, bool improved_solution) {

    mutationRate = mutRate;

    solution = new Solution(&(problem->exams), problem->timeslots, problem->students);
    //solution->initializeRandomSolution(true, improved_solution);
    solution->initializeRandomFeasibleSolution();

}

Chromosome::Chromosome(Problem* problem, int *initializingSolution) {

    mutationRate = mutRate;

    solution = new Solution(&problem->exams, problem->timeslots, problem->students, initializingSolution);

}

int *Chromosome::getGenes() {

    // Copy current genes
    int *genesCopy = new int[solution->exams->size()];
    std::copy(solution->examsTimeslots, solution->examsTimeslots + solution->exams->size(), genesCopy);

    return genesCopy;

}

void Chromosome::mutation(){
    /*     // Random stuff
    std::uniform_int_distribution<int> probabilityDistribution(0, 100);
    std::uniform_int_distribution<int> examsDistribution(0, solution->exams->size() - 1);
    std::uniform_int_distribution<int> timeslotsDistribution(0, solution->timeslots - 1);

    // Extract a random rate which must be compared with mutation rate
    float randomProbability = float(probabilityDistribution(generator)) / 100.0;

    // Randomly choose to mutate a gene
    if (randomProbability < mutationRate) {

        // Extract a random exam and a random slot to mutate
        int mutantExam = examsDistribution(generator);
        int mutantSlot = timeslotsDistribution(generator);

        // Store new timeslot for the random exam
        solution->examsTimeslots[mutantExam] = mutantSlot;

    } */
}

Chromosome* Chromosome::mutation(Problem* problem) {

    int MAX_TRIES = 10;

    std::uniform_int_distribution<int> examsDistribution(0, solution->exams->size() - 1);
    std::uniform_int_distribution<int> timeslotsDistribution(0, solution->timeslots - 1);

    Chromosome *child = new Chromosome(problem, (int*) getGenes());

    bool swapped = false;

    for(int i = 0; i < MAX_TRIES && !swapped; i++){
        int examIndex1 = examsDistribution(generator);
        int examIndex2;

        do{
            examIndex2 = examsDistribution(generator);
        } while(examIndex1 == examIndex2);

        Exam* exam1 = child->solution->exams->at(examIndex1);
        Exam* exam2 = child->solution->exams->at(examIndex2);

        int timeslot1 = exam1->timeslot;
        int timeslot2 = exam2->timeslot;

        double penalty = child->solution->calcPenaltyDelta(examIndex1, timeslot1, timeslot2) + child->solution->calcPenaltyDelta(examIndex2, timeslot2, timeslot1);
        bool feasible = !exam1->evaluateConflicts(child->solution->exams, timeslot2) && !exam2->evaluateConflicts(child->solution->exams, timeslot1);

        if(feasible && penalty < 0){
            child->solution->moveExam(exam1, timeslot2);
            child->solution->moveExam(exam1, timeslot2);
            swapped = true;
        }
    }

    return child;

}

void performStandardCrossover(Chromosome *firstParent, Chromosome *secondParent, Chromosome *firstChild, Chromosome *secondChild, int minCut, int maxCut){

    // Store genes pointer to clean code
    int *firstParentGenes = firstParent->solution->examsTimeslots;
    int *secondParentGenes = secondParent->solution->examsTimeslots;
    int *firstChildGenes = firstChild->solution->examsTimeslots;
    int *secondChildGenes = secondChild->solution->examsTimeslots;

    // Store exam size to perform better on iteration
    int numberOfExams = firstParent->solution->exams->size();

    for(int i = 0; i < numberOfExams; i++){
        if(i < minCut || i > maxCut){

            // Swap timeslots between solutions
            firstChildGenes[i] = secondParentGenes[i];
            secondChildGenes[i] = firstParentGenes[i];

        }
    }

    // Perform mutation on children
    firstChild->mutation();
    secondChild->mutation();

}

void performOrderedCrossover(Chromosome *firstParent, Chromosome *secondParent, Chromosome *firstChild, Chromosome *secondChild, int minCut, int maxCut, bool *performingCrossover){

    std::unordered_map<int, bool> firstParentUsedGenes, secondParentUsedGenes;

    // Store genes pointer to clean code
    int *firstParentGenes = firstParent->solution->examsTimeslots;
    int *secondParentGenes = secondParent->solution->examsTimeslots;
    int *firstChildGenes = firstChild->solution->examsTimeslots;
    int *secondChildGenes = secondChild->solution->examsTimeslots;

    // Store exam size to perform better on iteration
    int numberOfExams = firstParent->solution->exams->size();

    for(int i = maxCut; i < numberOfExams + minCut + 1; i++){
        if(i % numberOfExams < minCut || i % numberOfExams > maxCut){

            // Crossover for the first chromosome
            for(int j = i % numberOfExams, k = 0; *performingCrossover; j = (j + 1) % numberOfExams, k++){
                if(secondParentUsedGenes.find(j) == secondParentUsedGenes.end()) {

                    if(k >= maxCut - minCut + 1) {
                        *performingCrossover = false;
                        break;
                    }

                    // Try to set crossover solution
                    firstChildGenes[i] = secondParentGenes[j];
                    if (firstChild->solution->getFeasibility(false)) {
                        secondParentUsedGenes[j] = true;
                        break;
                    }

                }
            }

            for(int j = i % numberOfExams, k = 0; *performingCrossover; j = (j + 1) % numberOfExams, k++){
                if(firstParentUsedGenes.find(j) == firstParentUsedGenes.end()) {

                    if(k >= maxCut - minCut + 1) {
                        *performingCrossover = false;
                        break;
                    }

                    // Try to set crossover solution
                    secondChildGenes[i] = firstParentGenes[j];
                    if (secondChild->solution->getFeasibility(false)) {
                        firstParentUsedGenes[j] = true;
                        break;
                    }

                }
            }

        }
    }

    // Perform mutation on children
    // TODO: execute mutation with probability p(fitness)
    firstChild->mutation();
    secondChild->mutation();

}

Chromosome* Chromosome::crossover(Problem* problem, Chromosome *firstParent, Chromosome *secondParent) {

    std::uniform_int_distribution<int> timeslotDistribution(0, problem->timeslots - 1);
    int randomTimeslot = timeslotDistribution(generator);

    while(secondParent->solution->timeslotsExams[randomTimeslot].empty()){
        randomTimeslot = timeslotDistribution(generator);
    }

    std::uniform_int_distribution<int> examDistribution(0, secondParent->solution->timeslotsExams[randomTimeslot].size() - 1);
    int randomExamIndex = examDistribution(generator);
    int randomExam =  secondParent->solution->timeslotsExams[randomTimeslot][randomExamIndex];

    int correspondentTimeslot_parent1 = firstParent->solution->examsTimeslots[randomExam];

    double penaltyDelta = 0;

    for(int exam : secondParent->solution->timeslotsExams[randomTimeslot]){
        int timeslotSrc = firstParent->solution->examsTimeslots[exam];

        if(timeslotSrc != correspondentTimeslot_parent1)
            penaltyDelta += firstParent->solution->calcPenaltyDelta(randomExam, timeslotSrc, correspondentTimeslot_parent1);
    }

    Chromosome *firstChild = new Chromosome(problem, (int*) firstParent->getGenes());
    if(penaltyDelta < 0){
        
        for(int exam : secondParent->solution->timeslotsExams[randomTimeslot]){
            int timeslotSrc = firstParent->solution->examsTimeslots[exam];
            Exam* examObj = firstChild->solution->exams->at(exam);

            if(timeslotSrc != correspondentTimeslot_parent1 && !examObj->evaluateConflicts(firstChild->solution->exams, correspondentTimeslot_parent1))
                firstChild->solution->moveExam(firstChild->solution->exams->at(exam), correspondentTimeslot_parent1);
        }
    }

    return firstChild;
}

/* std::vector<Chromosome*> Chromosome::crossover(Problem* problem, Chromosome *firstParent, Chromosome *secondParent, bool ordered) {

    // Children collection
    std::vector<Chromosome*> children;
    int numberOfExams = firstParent->solution->exams->size();

    // Decide how many cuts will be according with the number of timeslot
    int minCut = 0, maxCut;
    int numberOfCuts = (firstParent->solution->timeslots > 10) ? 2 : 1;

    // Random stuff
    std::uniform_int_distribution<int> minCutDistribution(0, numberOfExams/2 - 1);
    std::uniform_int_distribution<int> maxCutDistribution(numberOfCuts > 1 ? numberOfExams/2 : 1, numberOfExams - 2);

    // Initialize children chromosomes
    Chromosome *firstChild = new Chromosome(problem, (int*) firstParent->getGenes());
    Chromosome *secondChild = new Chromosome(problem, (int*) secondParent->getGenes());

    // Extract
    maxCut = maxCutDistribution(generator);
    if(numberOfCuts == 2)
        minCut = minCutDistribution(generator);

    // Initialize flag for ordered crossover
    bool orderedCrossoverPerformed = true;

    if(ordered && firstParent->solution->getCutFeasibility(minCut, maxCut) && secondParent->solution->getCutFeasibility(minCut, maxCut))
        performOrderedCrossover(firstParent, secondParent, firstChild, secondChild, minCut, maxCut, &orderedCrossoverPerformed);
    else
        performStandardCrossover(firstParent, secondParent, firstChild, secondChild, minCut, maxCut);

    if(!orderedCrossoverPerformed)
        performStandardCrossover(firstParent, secondParent, firstChild, secondChild, minCut, maxCut);

    children.push_back(firstChild);
    children.push_back(secondChild);

    return children;

} */

Chromosome *Chromosome::inversion(Problem* problem) {

    // Random stuff
    std::uniform_int_distribution<int> cutDistribution(0, solution->exams->size() / 2 - 1);

    //Decide how many cuts will be according with the number of timeslot
    int cut = cutDistribution(generator);
    int numberOfExams = solution->exams->size();

    //Copy old genes and store solution pointer
    Chromosome *invertedChromosome = new Chromosome(problem, (int*) getGenes());

    int *invertedChromosomeGenes = invertedChromosome->solution->examsTimeslots;

    // Swap genes around cutting index
    for(int i = 0; i < cut; i++)
        std::swap(invertedChromosomeGenes[i], invertedChromosomeGenes[numberOfExams - 1 - i]);

    return invertedChromosome;

}

Chromosome *Chromosome::timeslot_inversion(Problem* problem) {

    // Random stuff
    std::uniform_int_distribution<int> cutDistribution(0, solution->timeslots - 1);

    //Decide how many cuts will be according with the number of timeslot
    int t1 = cutDistribution(generator);
    int t2 = cutDistribution(generator);

    while(t1 == t2)
        t2 = cutDistribution(generator);

    //Copy old genes and store solution pointer
    Chromosome *invertedChromosome = new Chromosome(problem, (int*) getGenes());

    for(int examId : solution->timeslotsExams[t1]){
        Exam *e = invertedChromosome->solution->exams->at(examId);
        invertedChromosome->solution->moveExam(e, t2);
    }
    for(int examId : solution->timeslotsExams[t2]){
        Exam *e = invertedChromosome->solution->exams->at(examId);
        invertedChromosome->solution->moveExam(e, t1);
    }

    return invertedChromosome;

}


//The fitness is calculated as an integer between 0 and nofGenes
double Chromosome::getFitness() {
    return solution->getGain();
}

Chromosome::~Chromosome() {
    delete solution;
}
