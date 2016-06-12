#include <mpi.h>
#include <stdio.h>
#include "Student.h"
#include "AlarmClock.h"
#include "settings.h"

int size, rank;

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    srand(time(NULL)+rank);
    if (size > 1) {
        if (rank > 0) {
            Student *student = new Student(size-1, rank);
            student->studentLoop();
            delete student;
        } else {
            AlarmClock *alarmClock = new AlarmClock(size-1, SLEEP_TIME, REPETITIONS);
            alarmClock->alarmLoop();
        }
    }

    MPI_Finalize();
}
