//
// Created by debian on 11.06.16.
//

#ifndef DRUNKSTUDENTS_ALARMCLOCK_H
#define DRUNKSTUDENTS_ALARMCLOCK_H

class AlarmClock {
private:
    unsigned int studentsNumber;
    unsigned int sleepTime;
    unsigned int repetitions;
public:
    AlarmClock(unsigned int studentsNumber, unsigned int sleepTime);
    AlarmClock(unsigned int studentsNumber, unsigned int sleepTime, unsigned int repetitions);
    ~AlarmClock();

    void alarmLoop();
};


#endif //DRUNKSTUDENTS_ALARMCLOCK_H
