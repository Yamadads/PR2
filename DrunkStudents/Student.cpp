/*
 * Student.cpp
 *
 *  Created on: 08-06-2016
 *      Author: debian
 */

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
    return result;
}



