#ifndef DATA_STRUCTURES_CPP
#define DATA_STRUCTURES_CPP

#include <string>
#include <vector>
#include <array>
#include <list>

#include "Exam.h"

class Solution {

public:

    std::vector<Exam*> *exams;
    int timeslots, students;

    /* [i]: timeslot assigned to exam i */
    int *examsTimeslots;

    /* [i]: list of exams scheduled in timeslot i */
    std::vector<std::vector<int>> timeslotsExams;

    /* Solution feasibility */
    bool isFeasible = true;

    std::vector<Exam*> examsNotPlaced;
    std::vector<Exam*> examsAlreadyPlaced;

    /* Constructor */
    explicit Solution(Solution* solution);
    Solution(std::vector<Exam*> *examsVector, int numberOfTimeslots, int numberOfStudents);
    Solution(std::vector<Exam*> *examsVector, int numberOfTimeslots, int numberOfStudents, const int *initializingSolution);

    /* Feasibility checker */
    bool getFeasibility(bool evaluatePenalty = true, int start = 0, int end = 0);
    bool getCutFeasibility(int minCut, int maxCut);
    void initializeRandomSolution(bool feasible = false, bool improved_solution = false);
    void initializeRandomFeasibleSolution();

    void moveExam(Exam* exam, int new_timeslot);

    double getPenalty();
    double getGain();
    void setPenalty(double new_penalty);

    void initExamsNotPlaced(std::vector<Exam*> sortedExams);
    std::vector<Exam*> organizeExams();
    std::vector<std::vector<int>> initializeTimeslotGroups();
    bool tryRandomPlacement(Exam* toBePlaced, std::vector<int> bucket);
    void computeRandomSchedule(std::vector<int> bucket);
    bool randomPlacement(Exam* toBePlaced, std::vector<int> bucket);
    bool isMoveFeasible(Exam* toBePlaced, int timeslot);
    bool randomMove(std::vector<int> bucket);
    void initSolution();
    double calcPenaltyDelta(int exam, int timeslotSrc, int timeslotDest);

    /**
     * Force re-computation of the penalty
     * Use it only if you're sure that the solution has changed, otherwise call getPenalty()
     */
    double computePenalty();

private:

    /* Penalty of the solution */
    bool computedPenalty = false;
    double penalty = 0.0;
};

#endif //DATA_STRUCTURES_CPP
