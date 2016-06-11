#include <mpi.h>
#include <stdio.h>
#include "Student.h"

int size, rank;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    Student* student = new Student(size, rank);
    printf("Student : %s \n", student->toString().c_str());
    delete student;
    MPI_Finalize();
}
