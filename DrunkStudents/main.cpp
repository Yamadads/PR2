#include <mpi.h>
#include "Student.h"
#include "AlarmClock.h"
#include "settings.h"

int size, rank;
int arbiters;

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(time(NULL) + rank);

    arbiters = 1;
    if (argc > 1) {
        try {
            arbiters = std::stoi(argv[1]);
            arbiters = std::max(arbiters,1);
        } catch (const std::invalid_argument &ia) {
            printf("Invalid argument: %s\n", ia.what());
            printf("Arbiters set to 1\n");
        }
    }

    if (size > 1) {
        if (rank > 0) {
            Student *student = new Student(size - 1, rank, arbiters);
            student->studentLoop();
            delete student;
        } else {
            AlarmClock *alarmClock = new AlarmClock(size - 1, SLEEP_TIME, REPETITIONS);
            alarmClock->alarmLoop();
        }
    }

    MPI_Finalize();
}
