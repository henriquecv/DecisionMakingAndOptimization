#include <algorithm>
#include <iterator>
#include <random>
#include <vector>
#include <limits>

#include "Exam.h"
#include "Solution.h"
#include "rand.h"

/* Solution class */


Solution::Solution(Solution *solution) : Solution(solution->exams, solution->timeslots, solution->students, solution->examsTimeslots) {}

Solution::Solution(std::vector<Exam*> *examsVector, int numberOfTimeslots, int numberOfStudents) {

    /* Store collection length */
    exams = examsVector;
    timeslots = numberOfTimeslots;
    students = numberOfStudents;

    /* Initialize exams vector */
    examsTimeslots = new int[exams->size()];

}

Solution::Solution(std::vector<Exam*> *examsVector, int numberOfTimeslots, int numberOfStudents, const int *initializingSolution){

    /* Store collection length */
    exams = examsVector;
    timeslots = numberOfTimeslots;
    students = numberOfStudents;

    /* Initialize timeslots/exams vector */
    for(int i = 0; i < timeslots; i++)
        timeslotsExams.emplace_back(std::vector<int>());

    /* Generate collections for exams/timeslots mapping and vice-versa */
    examsTimeslots = new int[exams->size()];
    for(int i = 0; i < examsVector->size(); ++i) {

        /* Copy exam's timeslot */
        examsTimeslots[i] = initializingSolution[i];

        /* Add exam to its timeslot */
        timeslotsExams[examsTimeslots[i]].push_back(i);

        exams->at(i)->timeslot = initializingSolution[i];

    }

};

bool Solution::getFeasibility(bool evaluatePenalty, int start, int end) {

    timeslotsExams.clear();
    for(int i = 0; i < timeslots; i++) {
        timeslotsExams.emplace_back(std::vector<int>());
    }
    for (int i=0; i<exams->size(); ++i) {
        timeslotsExams[examsTimeslots[i]].emplace_back(i);
    }

    for (int timeslot_id = 0; timeslot_id < timeslots; ++timeslot_id) {
        int timeslot_size = timeslotsExams[timeslot_id].size();

        for (int i = 0; i < timeslot_size; ++i) {
            int exam1_id = timeslotsExams[timeslot_id][i];

            for (int j = i + 1; j < timeslot_size; ++j) {
                int exam2_id = timeslotsExams[timeslot_id][j];

                if ((*exams)[exam1_id]->hasConflict(exam2_id)) {
                    return false;
                }

            }

        }

    }

    return true;

    /* OLD ONE - recheck */

    /* Populate timeslots/exam vector of lists */
/*     for(int i = start; i < (end > 0 ? end : exams->size()); i++)
        exams->at(i)->timeslot = examsTimeslots[i];

    for(int i = start; i < (end > 0 ? end : exams->size()) && isFeasible; i++)
        isFeasible = exams->at(i)->evaluateConflicts(exams);

    if(!isFeasible)
        penalty = 1000;
    else if(evaluatePenalty)
        penalty = getPenalty();

    return isFeasible; */

}

void clearExamVector(std::vector<Exam*> *vector){
    for (Exam *e : *vector)
        delete e;

    vector->clear();
}

bool Solution::getCutFeasibility(int minCut, int maxCut) {

    // Evaluate only cutting section feasibility
    return getFeasibility(false, minCut, maxCut);

}

void Solution::initExamsNotPlaced(std::vector<Exam*> sortedExams) {

    for(int i = 0; i < exams->size(); i++){
        Exam* exam = sortedExams[i]->copy();
        examsNotPlaced.push_back(exam);
    }

}

std::vector<Exam*> Solution::organizeExams(){

    std::vector<Exam*> sortedExams;

    for(int i = 0; i < exams->size(); i++){
        Exam* exam = exams->at(i)->copy();
        sortedExams.push_back(exam);
    }

    std::sort(sortedExams.begin(), sortedExams.end(), [](Exam *one, Exam *two) {
        return one->getConflicts() > two->getConflicts();
    });

    for(int i = 0; i < (sortedExams.size()/5); i++){
        std::random_shuffle(sortedExams.begin() + (5 * i), sortedExams.begin() + ((i + 1) * 5));
    }

    return sortedExams;
}

std::vector<std::vector<int>> Solution:: initializeTimeslotGroups(){

    std::vector<std::vector<int>> timeslotsGroups;
    std::vector<int> randomSequence(6);

    for(int i = 0; i < 6; i++)
        randomSequence[i] = i;
    
    std::shuffle(randomSequence.begin(), randomSequence.end(), generator);

    for(int i = 0; i < 6; i++){

        timeslotsGroups.emplace_back(std::vector<int>());

        int timeslot = randomSequence[i];

        while (timeslot < timeslots){
            timeslotsGroups[i].push_back(timeslot);
            timeslot += 6;
        }
    }

    return timeslotsGroups;
}

bool Solution::isMoveFeasible(Exam* examToBePlaced, int timeslot){

    // Scroll all exams in the current timeslot
    for (int k = 0; k < timeslotsExams[timeslot].size(); ++k) {
        int curr_exam_in_timeslot = timeslotsExams[timeslot][k];

        if(examToBePlaced->hasConflict(curr_exam_in_timeslot)) {
            return false;
        }
    }

    examToBePlaced->timeslot = timeslot;
    examsTimeslots[examToBePlaced->index] = timeslot;
    timeslotsExams[timeslot].emplace_back(examToBePlaced->index);

    return true;
}

bool Solution::randomPlacement(Exam* examToBePlaced, std::vector<int> timeslotsAvailable) {

    std::uniform_int_distribution<int> distribution(0, timeslotsAvailable.size() - 1);
    int randomIndex = distribution(generator);
    int destination = timeslotsAvailable[randomIndex];

    return isMoveFeasible(examToBePlaced, destination);
}

bool Solution::tryRandomPlacement(Exam* examToBePlaced, std::vector<int> timeslotsAvailable) {
    int PLACING_TRIES = 2 * timeslots;

    for (int k = 0; k < PLACING_TRIES; k++) {
        if (randomPlacement(examToBePlaced, timeslotsAvailable)) {
            examsNotPlaced.erase(std::remove(examsNotPlaced.begin(), examsNotPlaced.end(), examToBePlaced), examsNotPlaced.end());
            examsAlreadyPlaced.emplace_back(examToBePlaced);
            return true;
        }
    }
    return false;
}

void Solution::computeRandomSchedule(std::vector<int> timeslotsAvailable) {
    Exam* examToBePlaced;

    int i = 0;
    
    while (!examsNotPlaced.empty() && i < examsNotPlaced.size()) {
        examToBePlaced = examsNotPlaced[i];

        if (!tryRandomPlacement(examToBePlaced, timeslotsAvailable)) {
            i++;
            break;
        }
    }
}

bool Solution::randomMove(std::vector<int> timeslotsAvailable) {

    std::uniform_int_distribution<int> examDistribution(0, examsAlreadyPlaced.size() - 1);
    std::uniform_int_distribution<int> timeslotDistribution(0, timeslots - 1);

    int examIndex = examDistribution(generator);
    Exam* examToBeMoved = examsAlreadyPlaced[examIndex];

    int oldTimeslot = examToBeMoved->timeslot;
    int timeslot = timeslotDistribution(generator);

    while(oldTimeslot == timeslot)
        timeslot = timeslotDistribution(generator);

    bool result = isMoveFeasible(examToBeMoved, timeslot);

    if(result)
        timeslotsExams[oldTimeslot].erase(std::remove(timeslotsExams[oldTimeslot].begin(), timeslotsExams[oldTimeslot].end(), examToBeMoved->index), timeslotsExams[oldTimeslot].end());

    return result;
}

void Solution::initSolution(){
    /* Initialize timeslots/exams vector */
    timeslotsExams.clear();
    timeslotsExams.shrink_to_fit();

    for(int i = 0; i < timeslots; i++) {
        timeslotsExams.emplace_back(std::vector<int>());
    }

    /* Generate collections for exams/timeslots mapping and vice-versa */
    examsTimeslots = new int[exams->size()];
}

void Solution::initializeRandomFeasibleSolution(){
    int THRESHOLD = 1;
    int STUCK = 2;

    std::vector<Exam*> sortedExams = organizeExams();
    initExamsNotPlaced(sortedExams);

    std::vector<std::vector<int>> timeslotGroups = initializeTimeslotGroups();

    int tries = 0, imStuck = 0, timeslotGroup = 0;

    std::vector<int> timeslotsAvailable = timeslotGroups[timeslotGroup++];

    initSolution();

    while (!examsNotPlaced.empty()) {
        computeRandomSchedule(timeslotsAvailable);
        // I try to move randomly, until I manage to do it.

        bool randomFeasibleMove = false;

        while(!randomMove(timeslotsAvailable));

        tries++;
        // If I'm above the threshold
        if (tries > THRESHOLD) {
            // I try to expand my current bucket with a new one.
            if (timeslotGroup < 6) {
                timeslotsAvailable.insert(timeslotsAvailable.end(), timeslotGroups[timeslotGroup].begin(), timeslotGroups[timeslotGroup].end());
                timeslotGroup++;
            } else if (imStuck++ == STUCK) {
                // Restart
                imStuck = 0;
                timeslotGroup = 0;
                clearExamVector(&sortedExams);
                clearExamVector(&examsNotPlaced);
                clearExamVector(&examsAlreadyPlaced);
                sortedExams = organizeExams();
                initExamsNotPlaced(sortedExams);
                timeslotGroups = initializeTimeslotGroups();
                initSolution();
                THRESHOLD++;
                timeslotsAvailable = timeslotGroups[timeslotGroup++];
            }
            tries = 0;

        } 
    }

    //printf("Generated feasible solution \n");
}

/* void Solution::initializeRandomSolution(bool feasible, bool improved_solution) {

    if(feasible) {

        bool found_infeasibility = true;
        std::vector<int> tmp_examsTimeslots(exams->size());
        std::vector<int> shuffled_exams(exams->size());
        std::vector<int> shuffled_timeslots(timeslots);
        std::vector<Exam*> sortedExams = organizeExams();

        for (int i = 0; i < exams->size() || i < timeslots; ++i) {

            if(i < exams->size())
                shuffled_exams[i] = i;

            if(i < timeslots)
                shuffled_timeslots[i] = i;

        }

        while(found_infeasibility) {
            found_infeasibility = false;

            std::fill(tmp_examsTimeslots.begin(), tmp_examsTimeslots.end(), -1);

            // Initialize timeslots/exams vector 
            std::vector<std::vector<int>> tmp_timeslotsExams;
            tmp_timeslotsExams.reserve(timeslots);
            for(int i = 0; i < timeslots; ++i)
                tmp_timeslotsExams.emplace_back(std::vector<int>());

            std::shuffle(std::begin(shuffled_exams), std::end(shuffled_exams), generator);
            std::shuffle(std::begin(shuffled_timeslots), std::end(shuffled_timeslots), generator);

            // For each exam
            for (int i = 0; i < shuffled_exams.size() && !found_infeasibility; ++i){
                int curr_exam = shuffled_exams[i];

                // Search for a timeslot until you find one with no conflicts
                for (int j = 0; j < timeslots && tmp_examsTimeslots[curr_exam] == -1; ++j) {
                    int curr_timeslot = shuffled_timeslots[j];
                    found_infeasibility = false;

                    // Scroll all exams in the current timeslot
                    for (int k = 0; k < tmp_timeslotsExams[curr_timeslot].size() && !found_infeasibility; ++k) {
                        int curr_exam_in_timeslot = tmp_timeslotsExams[curr_timeslot][k];
                        if((*exams)[curr_exam]->hasConflict(curr_exam_in_timeslot)) {
                            found_infeasibility = true;
                        }
                    }

                    if (!found_infeasibility) {
                        tmp_examsTimeslots[curr_exam] = curr_timeslot;
                        tmp_timeslotsExams[curr_timeslot].emplace_back(curr_exam);
                    }
                }
            }
        }


        // Initialize timeslots/exams vector 
        timeslotsExams.clear();
        for(int i = 0; i < timeslots; i++) {
            timeslotsExams.emplace_back(std::vector<int>());
        }

        // Generate collections for exams/timeslots mapping and vice-versa 
        examsTimeslots = new int[exams->size()];
        for(int i = 0; i < exams->size(); ++i) {

            // Copy exam's timeslot 
            examsTimeslots[i] = tmp_examsTimeslots[i];

            // Add exam to its timeslot 
            timeslotsExams[examsTimeslots[i]].push_back(i);

        }

    } else {

        // Initialize randomizer
        std::uniform_int_distribution<int> distribution(0, timeslots - 1);

        for (int i = 0; i < exams->size(); i++) {
            // Extract random integer and fill up timeslot sample
            examsTimeslots[i] = distribution(generator);
        }
    }


} */

double Solution::calcPenaltyDelta(int exam, int timeslotSrc, int timeslotDest){
    double totalPenalty = 0;

    for(int i = 0; i < exams->size(); ++i){
        int timeslotsDistance = abs(timeslotDest - examsTimeslots[i]);
        if(timeslotsDistance <= 5 && (*exams)[exam]->hasConflict(i))
            totalPenalty += pow(2, (5 - timeslotsDistance)) * (*exams)[exam]->getConflict(i);
    }

    for(int j = 0; j < exams->size(); ++j){
        int timeslotsDistance = abs(timeslotSrc - examsTimeslots[j]);
        if(timeslotsDistance <= 5 && (*exams)[exam]->hasConflict(j))
            totalPenalty -= pow(2, (5 - timeslotsDistance)) * (*exams)[exam]->getConflict(j);
    }

    return totalPenalty;
}

double Solution::computePenalty() {

    double totalPenalty = 0;
    int timeslotsDistance;

    // Compute penalty
    for(int i = 0; i < exams->size(); ++i){
        for(int j = i + 1; j < exams->size(); ++j){
            timeslotsDistance = abs(examsTimeslots[i] - examsTimeslots[j]);
            if(timeslotsDistance <= 5 && (*exams)[i]->hasConflict(j))
                totalPenalty += pow(2, (5 - timeslotsDistance)) * (*exams)[i]->getConflict(j);
        }
    }

    totalPenalty = totalPenalty / students;

    // Set penalty
    setPenalty(totalPenalty);

    return totalPenalty;

}

double Solution::getPenalty() {
    return (computedPenalty ? penalty : computePenalty());
}

double Solution::getGain() {
    return (getPenalty() == 0 ? std::numeric_limits<double>::max() : 1.0/getPenalty());
}

void Solution::setPenalty(double newPenalty) {
    computedPenalty = true;
    penalty = newPenalty;
}

void Solution::moveExam(Exam *exam, int new_timeslot) {
    // TODO: see if penalty can be changed easily on the fly (in this case remove computePenalty() at the end of this function)
    // TODO: use moveExam() everywhere

    /* Remove exam from its old timeslot vector */
    std::vector<int> curr_timeslots = timeslotsExams[examsTimeslots[exam->index]];
    curr_timeslots.erase(std::remove(curr_timeslots.begin(), curr_timeslots.end(), exam->index), curr_timeslots.end()); // https://stackoverflow.com/a/3385251
    timeslotsExams[examsTimeslots[exam->index]] = curr_timeslots;

    /* Add exam to the new timeslot vector */
    timeslotsExams[new_timeslot].push_back(exam->index);

    /* Change the timeslot assigned to the exam */
    examsTimeslots[exam->index] = new_timeslot;

    exam->timeslot = new_timeslot;

    //computePenalty();
}