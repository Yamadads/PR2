/*
 * Student.cpp
 *
 *  Created on: 08-06-2016
 *      Author: debian
 */

#include <mpi.h>
#include "Student.h"
#include "Message.h"
#include "MessageTag.h"
#include "settings.h"

Student::Student(int studentsNumber, int studentID, int arbitersNumber) {
    this->studentsNumber = studentsNumber;
    this->studentID = studentID;
    arbitersQueue = new ArbitersQueue(arbitersNumber);
    lamport = new Lamport();
    groupID = 0;
    waitForArbiter = false;
    waitForOther = false;
    waitForArbiterInit = false;
    groupLamportTime = 0;
    setState(NOT_WANT_DRINK);
    lastMessages = new std::map<int, Message>();
    nextStateTime = time(0) + rand() % (std::max((MAX_STATE_TIME - MIN_STATE_TIME), (unsigned int) 1)) + MIN_STATE_TIME;
}

Student::~Student() {
    delete arbitersQueue;
    delete lamport;
    delete lastMessages;
}

std::string Student::toString() {
    std::string result;
    result = "Time: ";
    result.append(std::to_string(lamport->getTimestamp()));
    result.append(" ID: ");
    result.append(std::to_string(studentID));
    result.append(" state: ");
    result.append(stateName[actualState]);
    if (groupID > 0) {
        result.append(" groupID: ");
        result.append(std::to_string(groupID));
    }
    if (groupLamportTime > 0) {
        result.append(" groupLamportTime: ");
        result.append(std::to_string(groupLamportTime));
    }
    return result;
}

void Student::studentLoop() {
    Message message;
    MPI_Status status;
    bool running = true;
    while (running) {
        MPI_Recv(&message, sizeof(message), MPI_BYTE,
                 MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        switch (status.MPI_TAG) {
            case WAKE_UP:
                wakeUpMessage(running, message);
                break;
            case REQUEST:
                requestMessage(message);
                break;
            default:
                printf("Unknown MPI_TAG");
                break;
        }
    }
}

void Student::wakeUpMessage(bool &running, Message message) {
    //check if the end
    if (message.running == false) {
        running = false;
        return;
    }
    //check if change state
    if (nextStateTime != 0) {
        if (nextStateTime <= time(0)) {
            if (actualState == NOT_WANT_DRINK) {
                wantDrinkDecision();
            } else {
                if (actualState == DRINK) {
                    notWantDrinkDecision();
                }
            }
//            showStateInformation();
        }
    }
}


void Student::requestMessage(Message message) {
    lamport->update(message.timestamp);
    (*lastMessages)[message.studentID] = message;
    switch (message.state) {
        case NOT_WANT_DRINK:
            requestNotWantDrink(message);
            break;
        case WANT_DRINK:
            requestWantDrink(message);
            break;
        case WANT_ARBITER:
            requestWantArbiter(message);
            break;
        default:
            printf("Unknown state request");
            break;
    }
}

/**
 * Join to older group or send request to all students
 */
void Student::wantDrinkDecision() {
    setState(WANT_DRINK);
    groupLamportTime = 0;
    checkLocalMessages(lamport->getTimestamp());
    if (groupLamportTime != 0) {
        lamport->increment();
        Message message = setMessage();
        mpiCustomSend(message, groupID, REQUEST);
    } else {
        groupLamportTime = lamport->getTimestamp();
        askStudents();
        myFriends.clear();
        receiveReplyWantDrink();
        if (groupID == studentID) {
            if (myFriends.size() > 0) {
                wantArbiterState();
            } else {
                nextStateTime = 0;
                waitForOther = true;
            }
        } else {
            waitForArbiterInit = true;

        }
    }
}

void Student::showStateInformation() {
    printf("%s \n", toString().c_str());
}

void Student::notWantDrinkDecision() {
    arbitersQueue->removeRequest(groupLamportTime, groupID, studentID);
    setState(NOT_WANT_DRINK);
    lamport->increment();
    Message message = setMessage();
    for (int i = 1; i <= studentsNumber; i++) {
        if (i == studentID) continue;
        mpiCustomSend(message, i, REQUEST);
    }
    groupID = 0;
    groupLamportTime = 0;
    nextStateTime = time(0) + rand() % (std::max((MAX_STATE_TIME - MIN_STATE_TIME), (unsigned int) 1)) + MIN_STATE_TIME;
}

Message Student::setMessage() {
    Message message;
    message.studentID = studentID;
    message.timestamp = lamport->getTimestamp();
    message.groupID = groupID;
    message.groupLamportTime = groupLamportTime;
    message.running = true;
    message.state = actualState;
    return message;
}


bool Student::older(Message message) {
    if (message.groupLamportTime > groupLamportTime) return false;
    if (message.groupLamportTime == groupLamportTime) {
        if (message.groupID > studentID) return false;
    }
    return true;
}


void Student::setState(stateEnum state) {
    actualState = state;
    if (state == DRINK) {
        nextStateTime =
                time(0) + rand() % (std::max((MAX_STATE_TIME - MIN_STATE_TIME), (unsigned int) 1)) + MIN_STATE_TIME;
    }
    showStateInformation();
}

void Student::mpiCustomSend(Message message, int receiver, MessageTag tag) {
    MPI_Send(&message, sizeof(message), MPI_BYTE,
             receiver, tag, MPI_COMM_WORLD);
    printf("time: %i ID: %i state: %i receiverID: %i, groupID: %i, tag: %i\n", message.timestamp, studentID,
           message.state,
           receiver, message.groupID, tag);
}


void Student::checkLocalMessages(int time) {
    for (auto &message : *lastMessages) {
        if (message.second.state == WANT_DRINK) {
            if ((message.second.groupLamportTime < time) ||
                ((message.second.groupLamportTime == time) &&
                 (message.second.studentID < studentID))) {
                if (message.second.groupID == message.second.studentID) {
                    //if there is no group yet or is in younger group
                    if ((groupLamportTime == 0) || (groupLamportTime > message.second.groupLamportTime) ||
                        ((groupLamportTime == message.second.groupLamportTime) && (groupID > message.second.groupID))) {
                        groupID = message.second.groupID;
                        groupLamportTime = message.second.groupLamportTime;
                    }
                }
            }
        }
    }
}



void Student::askStudents() {
    lamport->increment();
    groupID = studentID;
    Message message = setMessage();
    for (int i = 1; i <= studentsNumber; i++) {
        if (i == studentID) {
            continue;
        }
        mpiCustomSend(message, i, REQUEST);
    }
}

void Student::receiveReplyWantDrink() {
    Message message;
    for (int i = 1; i <= studentsNumber; i++) {
        if (i == studentID) {
            continue;
        }
        MPI_Status status;
        customReceive(message,i,status);
        lamport->update(message.timestamp);
        if (message.state == WANT_DRINK) {
            if (older(message)) {
                groupID = message.groupID;
                groupLamportTime = message.groupLamportTime;
            } else {
                myFriends[message.studentID] = message.groupLamportTime;
            }
        }
    }
}

void Student::receiveReplyWantArbiter() {
    Message message;
    for (int i = 1; i <= studentsNumber; i++) {
        if (i == studentID) {
            continue;
        }
        MPI_Status status;
        customReceive(message,i,status);
        lamport->update(message.timestamp);
        arbitersQueue->clear();
        if (message.state == WANT_ARBITER) {
            arbitersQueue->addRequest(message);
        }
    }
}

void Student::requestNotWantDrink(Message message) {
    if (waitForArbiter) {
        arbitersQueue->removeRequest(message);
        if (arbitersQueue->canGetArbiter(groupLamportTime, groupID, studentID)) {
            waitForArbiter = false;
            setState(DRINK);
        }
    }
}

void Student::requestWantDrink(Message message) {
    if (waitForOther) {
        if (message.groupID == studentID) {
            lamport->increment();
            actualState = WANT_ARBITER;
            Message reply = setMessage();
            mpiCustomSend(reply, message.studentID, REPLY);
            wantArbiterState();
        } else {
            lamport->increment();
            Message reply = setMessage();
            mpiCustomSend(reply, message.studentID, REPLY);
        }
    } else {
        lamport->increment();
        Message reply = setMessage();
        mpiCustomSend(reply, message.studentID, REPLY);
    }
}

void Student::wantArbiterState() {
    setState(WANT_ARBITER);
    askStudents();
    myFriends.clear();
    receiveReplyWantArbiter();
    if (arbitersQueue->canGetArbiter(groupLamportTime, groupID, studentID)) {
        waitForArbiter = false;
        setState(DRINK);
    } else {
        waitForArbiter = true;
        nextStateTime = 0;
    }
}

void Student::requestWantArbiter(Message message) {
    if (waitForArbiterInit) {
        if ((message.groupID == groupID) && (message.groupLamportTime == groupLamportTime)) {
            actualState = WANT_ARBITER;
            lamport->increment();
            Message reply = setMessage();
            mpiCustomSend(reply, message.studentID, REPLY);
            wantArbiterState();
            waitForArbiterInit = false;
        }
    }else{
        lamport->increment();
        Message reply = setMessage();
        mpiCustomSend(reply, message.studentID, REPLY);
    }
}

void Student::customReceive(Message &message, int receiver, MPI_Status &status) {
    while(true){
        MPI_Recv(&message, sizeof(message), MPI_BYTE,
                 receiver, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        if (status.MPI_TAG==REPLY) return;
        if (status.MPI_TAG==REQUEST){
            lamport->update(message.timestamp);
            (*lastMessages)[message.studentID] = message;
            lamport->increment();
            Message reply = setMessage();
            mpiCustomSend(reply, message.studentID, REPLY);
        }
    }
}



































































