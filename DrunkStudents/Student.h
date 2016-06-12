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
#include "Message.h"
#include <string>
#include <vector>

class Student {
private:
	int studentsNumber;
	int studentID;

	stateEnum actualState;
	int groupID;
	int groupLamportTime;

	std::vector<Message> lastMessages;

	Lamport* lamport;
	long nextStateTime;

	void wakeUpMessage(bool &running, Message message);
	void requestMessage(Message message);
	void replyMessage(Message message);
	void wantDrinkDecision();
	void notWantDrinkDecision();
public:

	Student(int studentsNumber, int studentID);
	virtual ~Student();

	std::string toString();
	void studentLoop();
	void showStateInformation();
};

#endif /* STUDENT_H_ */
