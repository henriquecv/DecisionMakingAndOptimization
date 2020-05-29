#ifndef INC_2__PROGRAMMING_PROBLEM_H
#define INC_2__PROGRAMMING_PROBLEM_H

#include <string>
#include <vector>
#include <array>
#include <list>
#include <random>
#include "Exam.h"
#include "Solution.h"

class Problem {

public:

    // Name of the current instance
    std::string instanceName;

    // Time handling
    int max_time;
    int start_time;

    // Exams vector (containing conflicts)
    std::vector<Exam*> exams;

    // Number of timeslots
    int timeslots;

    // Number of students
    int students = 0;

    // Solutions for the current problem
    Solution* currentSolution = nullptr;
    Solution* bestSolution = nullptr;

    // Solution handling
    bool handleNewSolution(Solution *newSolution);

};

#endif //INC_2__PROGRAMMING_PROBLEM_H
