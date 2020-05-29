#ifndef IO_CPP
#define IO_CPP

/*** INPUT ***/

/*  Read the Exams file and return its content as a vector */
std::vector<Exam*> readExams(std::string examsInstance);

/* Get conflicts from enrolled students */
void retrieveConflicts(std::string studentsInstance, std::vector<Exam*> exams, int *students);

/*  Read the Timeslots file and return the number of timeslots  */
int readTimeslots(std::string timeslotsInstance);

/*  Main input function  */
Problem* getProblemFromFile(std::string instance_name, int max_time);

/*** OUTPUT ***/

void writeSolutionOnFile(Problem *p);

#endif //IO_CPP
