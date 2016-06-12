//
// Created by debian on 11.06.16.
//
#pragma once

#ifndef DRUNKSTUDENTS_MESSAGE_H
#define DRUNKSTUDENTS_MESSAGE_H
#include "StateEnum.h"

#endif //DRUNKSTUDENTS_MESSAGE_H

struct Message {
    int timestamp;
    int studentID;
    stateEnum state;
    int groupID;
    int groupLamportTime;
    bool running;
};