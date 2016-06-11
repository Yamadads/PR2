/*
 * Student.h
 *
 *  Created on: 08-06-2016
 *      Author: debian
 */

#pragma once

#ifndef STUDENT_H_
#define STUDENT_H_

#include "StateEnum.h"
#include "Lamport.h"
#include <string>

class Student {
private:
	int studentsNumber;
	int studentID;

	stateEnum actualState;
	int groupID;
	int groupLamportTime;

	Lamport* lamport;

public:

	Student(int studentsNumber, int studentID);
	virtual ~Student();

	std::string toString();
	void studentLoop();
};

#endif /* STUDENT_H_ */
