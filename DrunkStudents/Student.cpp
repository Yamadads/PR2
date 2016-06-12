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

Student::Student(int studentsNumber, int studentID) {
    this->studentsNumber = studentsNumber;
    this->studentID = studentID;
    lamport = new Lamport();
    needResponse = false;
    groupID = 0;
    requiredResponse = new std::vector<bool>(studentsNumber);
    groupLamportTime = 0;
    requiredResponse = 0;
    actualState = NOT_WANT_DRINK;
    lastMessages = new std::map<int,Message>();
    nextStateTime = time(0) + rand() % (MAX_STATE_TIME - MIN_STATE_TIME) + MIN_STATE_TIME;
}

Student::~Student() {
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
            showStateInformation();
        }
    }
}

void Student::requestMessage(Message message) {
    lamport->update(message.timestamp);
    (*lastMessages)[message.studentID] = message;
    switch(message.state){
        case NOT_WANT_DRINK:
            requestNotWantDrink(message);
            break;
        case WANT_DRINK:
            requestWantDrink(message);
            break;
        case WANT_ARBITER:
            requestWantArbiter(message);
            break;
        case DRINK:
            requestDrink(message);
            break;
        default:
            printf("Unknown state");
            break;
    }
}

/**
 * Join to older group or send request to all students
 */
void Student::wantDrinkDecision() {
    actualState = WANT_DRINK;
    for (auto &message : *lastMessages) {
        if (message.second.state == WANT_DRINK) {
            if ((message.second.groupLamportTime < lamport->getTimestamp()) ||
                ((message.second.groupLamportTime == lamport->getTimestamp()) &&
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
    //join to group
    if (groupLamportTime != 0) {
        lamport->increment();
        Message message = setMessage();
        MPI_Send(&message, sizeof(message), MPI_BYTE,
                 groupID, REQUEST, MPI_COMM_WORLD);
    } else {
        // ask all students if want drink
        lamport->increment();
        groupID = studentID;
        groupLamportTime = lamport->getTimestamp();
        Message message = setMessage();
        for (int i = 1; i <= studentsNumber; i++) {
            if (i==studentID){
                continue;
            }
            MPI_Send(&message, sizeof(message), MPI_BYTE,
                     i, REQUEST, MPI_COMM_WORLD);
        }
        //set that need response
        for (auto &e: *requiredResponse){
            e = true;
        }
        needResponse = true;
    }
    nextStateTime = 0;
}

void Student::showStateInformation() {
    printf("%s \n", toString().c_str());
}

void Student::notWantDrinkDecision() {
    actualState = NOT_WANT_DRINK;
    groupID = 0;
    groupLamportTime = 0;
    Message message = setMessage();
    for (int i = 1; i <= studentsNumber; i++) {
        MPI_Send(&message, sizeof(message), MPI_BYTE,
                 i, REQUEST, MPI_COMM_WORLD);
    }

    //TODO sekcja krytyczna
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
/*
void Student::responseNotWantDrink(Message message) {
    if (message.state==NOT_WANT_DRINK){
        //TODO zwolnij zasób arbitra
    }



}*/

void Student::replyMessage(Message message) {

}

void Student::requestNotWantDrink(Message message) {
    //TODO zwolnić zasób arbitra w lokalnej kolejce to samo we wszystkich stanach
}

void Student::requestWantDrink(Message message) {
    if (actualState==WANT_DRINK){
    //TODO sprawdzać grupy
    }else{
        sendReplyWithState(message);
    }
}

void Student::requestWantArbiter(Message message) {

}

void Student::requestDrink(Message message) {
    
}

void Student::sendReplyWithState(Message message) {
    lamport->increment();
    Message response = setMessage();
    MPI_Send(&response, sizeof(response), MPI_BYTE,
             groupID, REPLY, MPI_COMM_WORLD);
}

































