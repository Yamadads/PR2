//
// Created by debian on 13.06.16.
//

#ifndef DRUNKSTUDENTS_QUEUE_H
#define DRUNKSTUDENTS_QUEUE_H

#include <map>
#include <set>
#include <vector>


class ArbitersQueue {
private:
    std::map<int, std::map<int, std::set<int>*>*> *requests;
    int arbitersNumber;

public:
    ArbitersQueue(int arbitersNumber);
    ~ArbitersQueue();
    void addRequest(int groupTimestamp, int groupID, int studentID);
    void removeRequest(int groupTimestamp, int groupID, int studentID);
    bool canGetArbiter(int groupTimestamp, int groupID, int studentID);
};


#endif //DRUNKSTUDENTS_QUEUE_H
