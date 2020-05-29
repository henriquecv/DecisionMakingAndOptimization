#ifndef EXAM_CPP
#define EXAM_CPP

#include <unordered_map>

class Exam {

public:

    int id, index, timeslot;
    int enrolledStudents;
    std::unordered_map<int, int> conflicts;

    /* Constructor */
    Exam(int examId, int examIndex, int enrolledStudentsIO);

    /* Conflict handling */
    void setConflict(int conflictingExamId);
    int getConflict(int examId);
    int getConflicts();
    bool hasConflict(int examId);
    bool evaluateConflicts(std::vector<Exam*> *exams, int destTimeslot);
    void printAllConflicts(std::vector<Exam*> *exams, bool useIndex = false);

    /* Clone instance */
    Exam* copy();

};


#endif //EXAM_CPP
