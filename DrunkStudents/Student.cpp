/*
 * Student.cpp
 *
 *  Created on: 08-06-2016
 *      Author: debian
 */

#include <mpi.h>
#include "Student.h"

Student::Student(int studentsNumber, int studentID) {
    this->studentsNumber = studentsNumber;
    this->studentID = studentID;
    lamport = new Lamport();
    groupID = 0;
    groupLamportTime = 0;
    actualState = NOT_WANT_DRINK;
}

Student::~Student() {
}

std::string Student::toString() {
    std::string result;
    result = "ID: ";
    result.append(std::to_string(studentID));
    result.append(" state: ");
    result.append(stateName[actualState]);
    if (groupID>0){
        result.append(" groupID: ");
        result.append(std::to_string(groupID));
    }
    if (groupLamportTime>0){
        result.append(" groupLamportTime: ");
        result.append(std::to_string(groupLamportTime));
    }
    return result;
}

void Student::studentLoop() {
    bool message;
    MPI_Status status;
    while(true){
        MPI_Recv(&message, sizeof(message), MPI_C_BOOL,
                 MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        printf("Student : %s \n", toString().c_str());
        if (message==false){
            break;
        }
    }
}





