#include <mpi.h>
#include <stdio.h>
#include "Student.h"
#include "AlarmClock.h"

int size, rank;

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size > 1) {
        if (rank > 0) {
            Student *student = new Student(size, rank);
            student->studentLoop();
            delete student;
        } else {
            AlarmClock *alarmClock = new AlarmClock(size-1, 5);
            alarmClock->alarmLoop();
        }
    }

    MPI_Finalize();
}
