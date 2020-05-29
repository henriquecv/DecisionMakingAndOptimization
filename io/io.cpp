#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "../data-structures/Exam.h"
#include "../data-structures/Problem.h"

/*  Read the Exams file and return its content as a vector */
std::vector<Exam*> readExams(std::string examsInstance){

    // Setting up exams dynamic vector
    std::vector<Exam*> exams;

    // File IO
    std::ifstream file;
    file.open(examsInstance, std::ios_base::in);

    int examId, examEnrolledStudents;
    int current_exam_index = -1;

    if (!file)
        std::cout << "Unable to open file ";
    else {
        while(file >> examId >> examEnrolledStudents)
            exams.push_back(new Exam(examId, ++current_exam_index, examEnrolledStudents));
    }

    // Close file
    file.close();

    return exams;

}

/* Get conflicts from enrolled students */
void retrieveConflicts(std::string studentsInstance, std::vector<Exam*> exams, int *students) {

    // File IO
    std::ifstream file;
    file.open(studentsInstance, std::ios_base::in);

    // Iteration variables
    std::string prevStudentId;
    std::string currentStudentId;
    int examId;

    if (!file)
        std::cout << "Unable to open file";
    else {

        // Create a collection to store conflicting exams for each student
        std::vector<int> conflictingExams;

        while(file >> currentStudentId >> examId) {

            // Set conflicts on equal student id, otherwise clear conflicts vector and update student ID
            if(currentStudentId == prevStudentId){
                for(auto& conflictingExam: conflictingExams){
                    exams[conflictingExam - 1]->setConflict(examId - 1);
                    exams[examId - 1]->setConflict(conflictingExam - 1);
                }
            } else {
                (*students)++;
                conflictingExams.clear();
            }

            conflictingExams.push_back(examId);
            prevStudentId = currentStudentId;

        }

    }

    // Close file
    file.close();

}

/*  Read the Timeslots file and return the number of timeslots  */
int readTimeslots(std::string timeslotsInstance){

    int timeslots;

    // File IO
    std::ifstream file;
    file.open(timeslotsInstance, std::ios_base::in);

    if (!file)
        std::cout << "Unable to open file";

    file >> timeslots;

    // Close file
    file.close();

    return timeslots;

}

/*  Main input function  */
Problem* getProblemFromFile(std::string instance_name, int max_time) {

    auto* p = new Problem;

    p->instanceName = std::move(instance_name);
    p->max_time = max_time;
    p->start_time = time(nullptr);

    // Read the files
    p->exams = readExams(p->instanceName + ".exm");
    p->timeslots = readTimeslots(p->instanceName + ".slo");
    retrieveConflicts(p->instanceName + ".stu", p->exams, &p->students);

    return p;

}

/*** OUTPUT ***/

/*  Main output function  */
void writeSolutionOnFile(Problem *p) {

    // File IO
    std::ofstream file;
    file.open(p->instanceName + "_DMOgroup03.sol");

    for(int i = 0; i < p->bestSolution->exams->size(); i++) {
        int exam_id = p->bestSolution->exams->at(i)->id;
        int timeslot_id = p->bestSolution->examsTimeslots[i] + 1; // timeslots start from 1

        file << exam_id << " " << timeslot_id << "\n";
    }

    file.close();

}