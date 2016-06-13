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
    waitingForArbiter = false;
    arbitersQueue = new ArbitersQueue(arbitersNumber);
    lamport = new Lamport();
    needResponse = false;
    groupID = 0;
    groupSize = 0;
    oldGroupLamportTime = 0;
    requiredResponse = new std::vector<bool>(studentsNumber);
    groupLamportTime = 0;
    joinLamportTime = 0;
    setState(NOT_WANT_DRINK);
    lastMessages = new std::map<int, Message>();
    nextStateTime = time(0) + rand() % (MAX_STATE_TIME - MIN_STATE_TIME) + MIN_STATE_TIME;
}

Student::~Student() {
    delete arbitersQueue;
    delete lamport;
    delete requiredResponse;
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
            case REPLY:
                replyMessage(message);
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
                wantDrinkDecision(lamport->getTimestamp()+1);
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
void Student::wantDrinkDecision(int time) {
    setState(WANT_DRINK);
    groupLamportTime = 0;
    checkLocalMessages(time);
    //join to group
    if (groupLamportTime != 0) {
        lamport->increment();
        Message message = setMessage();
        mpiCustomSend(message, groupID, REQUEST);
        joinLamportTime = lamport->getTimestamp();
        oldGroupLamportTime = time;
    } else {
        // ask all students if want drink
        lamport->increment();
        groupID = studentID;
        groupLamportTime = time;
        oldGroupLamportTime = groupLamportTime;
        Message message = setMessage();
        for (int i = 1; i <= studentsNumber; i++) {
            if (i == studentID) {
                continue;
            }
            mpiCustomSend(message, i, REQUEST);
        }
        setNeedResponse();
    }
    nextStateTime = 0;
}

void Student::showStateInformation() {
    printf("%s \n", toString().c_str());
}

void Student::notWantDrinkDecision() {
    arbitersQueue->removeRequest(groupLamportTime, groupID, studentID);
    setState(NOT_WANT_DRINK);
    Message message = setMessage();
    for (int i = 1; i <= studentsNumber; i++) {
        if (i == studentID) continue;
        mpiCustomSend(message, i, REQUEST);
    }
    groupID = 0;
    groupLamportTime = 0;
    nextStateTime = time(0) + rand() % (MAX_STATE_TIME - MIN_STATE_TIME) + MIN_STATE_TIME;
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

void Student::requestNotWantDrink(Message message) {
    arbitersQueue->removeRequest(message.groupLamportTime, message.groupID, message.studentID);
    if (waitingForArbiter) {
        if (arbitersQueue->canGetArbiter(groupLamportTime, groupID, studentID)) {
            setState(DRINK);
            waitingForArbiter = false;
            nextStateTime = time(0) + rand() % (MAX_STATE_TIME - MIN_STATE_TIME) + MIN_STATE_TIME;
        }
    }
}

void Student::requestWantDrink(Message message) {
    if (actualState == WANT_DRINK) {
        if (older(message)) {
            groupID = message.groupID;
            groupLamportTime = message.groupLamportTime;
        }
        if (!needResponse) {
            if (message.groupID == studentID) {
                lamport->increment();
                groupID = message.groupID;
                groupLamportTime = message.groupLamportTime;
                for (int i = 1; i <= studentsNumber; i++) {
                    if (i == studentID) continue;
                    setState(WANT_ARBITER);
                    Message response = setMessage();
                    mpiCustomSend(response, i, REQUEST);
                }
            }
        }else{

        }
    } else {
        sendReplyWithState(message);
    }
}

void Student::requestWantArbiter(Message message) {
    arbitersQueue->addRequest(message.groupLamportTime, message.groupID, message.studentID);
    if (actualState == WANT_DRINK) {
        if ((message.studentID == groupID) && (message.groupID == groupID) &&
            (message.groupLamportTime == groupLamportTime)) {
            if (message.timestamp > joinLamportTime) {
                setState(WANT_ARBITER);
                lamport->increment();
                Message response = setMessage();
                mpiCustomSend(response, message.studentID, REPLY);
                lamport->increment();
                for (int i = 1; i <= studentsNumber; i++) {
                    if (i == studentID) continue;
                    Message response = setMessage();
                    mpiCustomSend(response, i, REQUEST);
                }
                setNeedResponse();

            } else { //need to start again
                groupID = studentID;
                wantDrinkDecision(oldGroupLamportTime);
            }
        }
    }
    sendReplyWithState(message);
}

void Student::sendReplyWithState(Message message) {
    lamport->increment();
    Message response = setMessage();
    mpiCustomSend(response, message.studentID, REPLY);
}

bool Student::older(Message message) {
    if (message.groupID == 0) return false;
    if (message.groupLamportTime > groupLamportTime) return false;
    if (message.groupLamportTime == groupLamportTime) {
        if (message.groupID > studentID) return false;
    }
    return true;
}

bool Student::responseComplete() {
    int count = 0;
    for (int i = 0; i < requiredResponse->size(); i++) {
        if ((*requiredResponse)[i]) {
            count++;
        }
    }
    if (count == 1) return true;
    return false;
}

void Student::replyMessage(Message message) {
    lamport->update(message.timestamp);
    (*lastMessages)[message.studentID] = message;
    switch (actualState) {
        case WANT_DRINK:
            replyWantDrink(message);
            break;
        case WANT_ARBITER:
            replyWantArbiter(message);
            break;
        default:
            printf("Unknown state reply %i\n", actualState );
    }
}

void Student::replyWantDrink(Message message) {
    if (message.groupID == studentID) {
        groupSize++;
    }
    (*requiredResponse)[message.studentID - 1] = false;
    if (older(message)) {
        groupID = message.groupID;
        groupLamportTime = message.groupLamportTime;
    }
    if (responseComplete()) {
        needResponse = false;
        if (groupID != studentID) {
            lamport->increment();
            Message response = setMessage();
            mpiCustomSend(response, groupID, REQUEST);
            joinLamportTime = lamport->getTimestamp();
        } else {
            if (groupSize > 0) {
                lamport->increment();
                for (int i = 1; i <= studentsNumber; i++) {
                    if (i == studentID) continue;
                    setState(WANT_ARBITER);
                    Message response = setMessage();
                    mpiCustomSend(response, i, REQUEST);
                }
            }
        }
    }
}

void Student::replyWantArbiter(Message message) {
    if (message.state == WANT_ARBITER) {
        arbitersQueue->addRequest(message.groupLamportTime, message.groupID, message.studentID);
    }
    (*requiredResponse)[message.studentID - 1] = false;
    if (responseComplete()) {
        needResponse = false;
        if (arbitersQueue->canGetArbiter(groupLamportTime, groupID, studentID)) {
            waitingForArbiter = false;
            setState(DRINK);
            nextStateTime = time(0) + rand() % (MAX_STATE_TIME - MIN_STATE_TIME) + MIN_STATE_TIME;
        } else {
            waitingForArbiter = true;
        }
    }
}

void Student::setState(stateEnum state) {
    actualState = state;
    showStateInformation();
}

void Student::mpiCustomSend(Message message, int receiver, MessageTag tag) {
    MPI_Send(&message, sizeof(message), MPI_BYTE,
             receiver, tag, MPI_COMM_WORLD);
    printf("Time: %i ID: %i state: %i receiverID: %i, groupID: %i, tag: %i\n", message.timestamp, studentID, message.state,
           receiver, message.groupID, tag);
}

void Student::setNeedResponse() {
    for (int i = 0; i < requiredResponse->size(); i++) {
        (*requiredResponse)[i] = true;
    }
    groupSize = 0;
    needResponse = true;
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



















































