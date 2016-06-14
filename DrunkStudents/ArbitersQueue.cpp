//
// Created by debian on 13.06.16.
//

#include "ArbitersQueue.h"


ArbitersQueue::ArbitersQueue(int arbitersNumber) {
    this->arbitersNumber = arbitersNumber;
    requests = new std::map<std::string, int>();
    help = new std::map<std::string, id>();

}

void ArbitersQueue::addRequest(int groupTimestamp, int groupID, int studentID) {
    id addID = getID(groupTimestamp, groupID);
    std::string strID = getIDstr(groupTimestamp, groupID);
    (*help)[strID]=addID;
    if (requests->count(strID)==0){
        (*requests)[strID]=1;
    }else{
        (*requests)[strID]=(*requests)[strID]+1;
    }
}

void ArbitersQueue::removeRequest(int groupTimestamp, int groupID, int studentID) {
    id addID = getID(groupTimestamp, groupID);
    std::string strID = getIDstr(groupTimestamp, groupID);
    if (requests->count(strID)!=0){
        (*requests)[strID]=(*requests)[strID]-1;
        if ((*requests)[strID]==0){
            requests->erase(strID);
            help->erase(strID);
        }
    }
//    printf("size %i\n", requests->size());
}

bool ArbitersQueue::canGetArbiter(int groupTimestamp, int groupID, int studentID) {
    id addID = getID(groupTimestamp, groupID);
    std::string strID = getIDstr(groupTimestamp, groupID);
    if (requests->count(strID)!=0){
        int count = 0;
        for (auto &e: *help){
            if (e.second.time<groupTimestamp){
                count++;
            }
            if ((e.second.time==groupTimestamp)&&(e.second.groupId<groupID)){
                count++;
            }
        }
//        printf("count %i\n", count);
        if ((count)<arbitersNumber){
            return true;
        }
        return false;
    }
}

ArbitersQueue::~ArbitersQueue() {
    delete requests;
    delete help;
}

id ArbitersQueue::getID(int groupTimestamp, int groupID) {
    id addID;
    addID.time = groupTimestamp;
    addID.groupId = groupID;
    return addID;
}

std::string ArbitersQueue::getIDstr(int groupTimestamp, int groupID) {
    return std::to_string(groupTimestamp)+"-"+std::to_string(groupID);
}

void ArbitersQueue::clear() {
    delete requests;
    delete help;
    requests = new std::map<std::string, int>();
    help = new std::map<std::string, id>();
}

void ArbitersQueue::addRequest(Message message) {
    addRequest(message.groupLamportTime, message.groupID, message.studentID);
}

void ArbitersQueue::removeRequest(Message message) {
    removeRequest(message.groupLamportTime, message.groupID, message.studentID);
}

bool ArbitersQueue::canGetArbiter(Message message) {
    return canGetArbiter(message.groupLamportTime, message.groupID, message.studentID);
}





















