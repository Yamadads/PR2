//
// Created by debian on 11.06.16.
//

#include "AlarmClock.h"
#include <unistd.h>
#include <mpi.h>
#include "MessageTag.h"
#include "Message.h"

AlarmClock::AlarmClock(unsigned int studentsNumber, unsigned int sleepTime) {
    this->studentsNumber = studentsNumber;
    this->sleepTime = sleepTime;
    repetitions = 0;
}

AlarmClock::AlarmClock(unsigned int studentsNumber, unsigned int sleepTime, unsigned int repetitions) {
    this->studentsNumber = studentsNumber;
    this->sleepTime = sleepTime;
    this->repetitions = repetitions;
}

void AlarmClock::alarmLoop() {
    Message message;
    message.running=true;
    if (repetitions == 0) {
        while (true) {
            sleep(sleepTime);
            for (int i = 1; i <= studentsNumber; i++) {
                MPI_Send(&message, sizeof(message), MPI_BYTE,
                         i, WAKE_UP, MPI_COMM_WORLD);
            }
        }
    }else{
        while(repetitions>0){
            repetitions--;
            sleep(sleepTime);
            for (int i = 1; i <= studentsNumber; i++) {
                MPI_Send(&message, sizeof(message), MPI_BYTE,
                         i, WAKE_UP, MPI_COMM_WORLD);
            }
        }
        message.running = false;
        for (int i = 1; i <= studentsNumber; i++) {
            MPI_Send(&message, sizeof(message), MPI_BYTE,
                     i, WAKE_UP, MPI_COMM_WORLD);
        }
    }


}