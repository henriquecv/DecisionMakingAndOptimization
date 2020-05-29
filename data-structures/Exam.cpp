#include <vector>

#include "Exam.h"

/* Constructor */
Exam::Exam(int examId, int examIndex, int enrolledStudentsIO) {

    id = examId;
    index = examIndex;
    enrolledStudents = enrolledStudentsIO;

}

/* Conflict handling */
void Exam::setConflict(int conflictingExamIndex) {

    if(conflicts.find(conflictingExamIndex) == conflicts.end())
        conflicts[conflictingExamIndex] = 0;

    conflicts[conflictingExamIndex]++;

}

int Exam::getConflicts(){
    int i = 0;

    for(auto& conflict : conflicts){
        if(conflict.second > 0)
            i++;
    }

    return i;
}

int Exam::getConflict(int examIndex) {
    return conflicts[examIndex];
}

bool Exam::hasConflict(int examIndex) {
    return (conflicts.find(examIndex) != conflicts.end());
}

bool Exam::evaluateConflicts(std::vector<Exam*> *exams, int destTimeslot) {

    int e = 0;
    bool thisHasConflict = false;

    for(auto& conflict : conflicts){
        if(conflict.second > 0)
            thisHasConflict = (destTimeslot == exams->at(conflict.first)->timeslot);

        if(thisHasConflict)
            break;
    }

    return thisHasConflict;

}

void Exam::printAllConflicts(std::vector<Exam*> *exams, bool useIndex) {

    /* Choose index or id and print conflicts */
    printf("Conflicts for exam %d: ", useIndex ? index : id);
    for(auto& conflictingExam : conflicts)
        printf("%d ", useIndex ? exams->at(conflictingExam.first)->index : exams->at(conflictingExam.first)->id);

}

Exam* Exam::copy(){
    Exam* exam = new Exam(id, index, enrolledStudents);
    exam->timeslot = timeslot;

    for(auto& conflict : conflicts){
        exam->conflicts[conflict.first] = conflict.second;
    }

    return exam;
}