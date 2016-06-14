//
// Created by debian on 13.06.16.
//

#ifndef DRUNKSTUDENTS_QUEUE_H
#define DRUNKSTUDENTS_QUEUE_H

#include <map>
#include <set>
#include <vector>
#include "Message.h"

struct id{
    int time;
    int groupId;
};

class ArbitersQueue {
private:
    std::map<std::string, int> *requests;
    std::map<std::string, id> *help;
    int arbitersNumber;
    id getID(int groupTimestamp, int groupID);
    std::string getIDstr(int groupTimestamp, int groupID);
public:
    ArbitersQueue(int arbitersNumber);
    ~ArbitersQueue();
    void addRequest(Message message);
    void addRequest(int groupTimestamp, int groupID, int studentID);
    void removeRequest(Message message);
    void removeRequest(int groupTimestamp, int groupID, int studentID);
    bool canGetArbiter(Message message);
    bool canGetArbiter(int groupTimestamp, int groupID, int studentID);
    void clear();
};


#endif //DRUNKSTUDENTS_QUEUE_H
