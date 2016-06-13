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
#include "MessageTag.h"
#include <string>
#include <map>
#include <vector>

class Student {
private:
	int studentsNumber;
	int studentID;

	stateEnum actualState;
	int groupID;
	int groupLamportTime;
	int joinLamportTime;
	int groupSize;

	int oldGroupLamportTime;

	std::map<int,Message> *lastMessages;
	bool needResponse;
	std::vector<bool> *requiredResponse;
	Lamport* lamport;
	long nextStateTime;

	void wakeUpMessage(bool &running, Message message);
	void requestMessage(Message message);
	void replyMessage(Message message);

	void requestNotWantDrink(Message message);
	void requestWantDrink(Message message);
	void requestWantArbiter(Message message);

	void replyWantDrink(Message message);
	void replyWantArbiter(Message message);

	void wantDrinkDecision();
	void notWantDrinkDecision();

	Message setMessage();
	void sendReplyWithState(Message message);
	bool older(Message message);
	bool responseComplete();
	void mpiCustomSend(Message message, int receiver,MessageTag tag);
public:

	Student(int studentsNumber, int studentID);
	virtual ~Student();

	std::string toString();
	void studentLoop();
	void showStateInformation();
	void setState(stateEnum state);
};

#endif /* STUDENT_H_ */
