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
#include "ArbitersQueue.h"
#include <string>
#include <map>
#include <vector>
#include <mpi.h>

class Student {
private:
    int studentsNumber;
    int studentID;

    ArbitersQueue *arbitersQueue;

    stateEnum actualState;
    int groupID;
    int groupLamportTime;


    std::map<int, Message> *lastMessages;
    Lamport *lamport;
    long nextStateTime;
    std::map<int, int> myFriends;
    bool waitForArbiter;
    bool waitForOther;
    bool waitForArbiterInit;

    void wakeUpMessage(bool &running, Message message);

    void requestMessage(Message message);

    void requestNotWantDrink(Message message);

    void requestWantDrink(Message message);

    void requestWantArbiter(Message message);

    void wantDrinkDecision();

    void notWantDrinkDecision();

    Message setMessage();

    bool older(Message message);

    void mpiCustomSend(Message message, int receiver, MessageTag tag);

    void setState(stateEnum state);

    void checkLocalMessages(int time);

    void askStudents();

    void receiveReplyWantDrink();

    void receiveReplyWantArbiter();

    void wantArbiterState();

    void customReceive(Message &message,int receiver, MPI_Status &status);
public:

    Student(int studentsNumber, int studentID, int arbitersNumber);

    virtual ~Student();

    std::string toString();

    void studentLoop();

    void showStateInformation();

};

#endif /* STUDENT_H_ */
