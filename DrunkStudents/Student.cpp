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
    groupID = 0;
    groupLamportTime = 0;
    actualState = NOT_WANT_DRINK;
    nextStateTime = time(0)+rand()%(MAX_STATE_TIME-MIN_STATE_TIME)+MIN_STATE_TIME;
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
    if (groupID>0){
        result.append(" groupID: ");
        result.append(std::to_string(groupID));
    }
    if (groupLamportTime>0){
        result.append(" groupLamportTime: ");
        result.append(std::to_string(groupLamportTime));
    }
    result.append(std::to_string(nextStateTime));
    return result;
}

void Student::studentLoop() {
    Message message;
    MPI_Status status;
    bool running = true;
    while(running){
        MPI_Recv(&message, sizeof(message), MPI_BYTE,
                 MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        switch (status.MPI_TAG){
            case WAKE_UP:
                wakeUpMessage(running, message);
                break;
            case REQUEST:
                requestMessage(message);
                break;
            case REPLY:
                replyMessage(message);
            default: break;
        }
    }
}

void Student::wakeUpMessage(bool &running, Message message) {
    //check if the end
    if (message.running==false){
        running = false;
        return;
    }
    //check if change state
    if (nextStateTime!=0){
        if (nextStateTime<=time(0)){
            if (actualState==NOT_WANT_DRINK){
                wantDrinkDecision();
            }else{
                notWantDrinkDecision();
            }
            showStateInformation();
        }
    }
}

void Student::requestMessage(Message message) {

}

void Student::replyMessage(Message message) {

}

void Student::wantDrinkDecision() {
    actualState = WANT_DRINK;
    for (auto &message : lastMessages){
        printf(" id: %i",message.studentID);
    }
    nextStateTime = time(0)+rand()%(MAX_STATE_TIME-MIN_STATE_TIME)+MIN_STATE_TIME;
    //TODO informacja do wszystkich że chcę pić
    //sprawdzenie czy ktoś nie proponował picia i nie jest starszy ode mnie
}

void Student::showStateInformation() {
    printf("%s \n", toString().c_str());
}

void Student::notWantDrinkDecision() {
    actualState = NOT_WANT_DRINK;
    //TODO informacja do wszystkich, że skończyłem pić
    //zwolnienie zasobu
    nextStateTime = time(0)+rand()%(MAX_STATE_TIME-MIN_STATE_TIME)+MIN_STATE_TIME;
}

















