//
// Created by debian on 13.06.16.
//

#include "ArbitersQueue.h"

ArbitersQueue::ArbitersQueue(int arbitersNumber) {
    this->arbitersNumber = arbitersNumber;
    requests = new std::map<int, std::map<int, std::set<int> *> *>();
}

void ArbitersQueue::addRequest(int groupTimestamp, int groupID, int studentID) {
//    printf("before add %i\n", requests->size());
    if (requests->find(groupTimestamp) != requests->end()) {
        if ((*requests)[groupTimestamp]->find(groupID) != (*requests)[groupID]->end()) {
            if ((*(*requests)[groupTimestamp])[groupID]->find(studentID) ==
                (*(*requests)[groupTimestamp])[groupID]->end()) {
                (*(*requests)[groupTimestamp])[groupID]->insert(studentID);
            }
        } else {
            (*(*requests)[groupTimestamp])[groupID] = new std::set<int>();
            (*(*requests)[groupTimestamp])[groupID]->insert(studentID);
        }
    } else {
        (*requests)[groupTimestamp] = new std::map<int, std::set<int> *>();
        (*(*requests)[groupTimestamp])[groupID] = new std::set<int>();
        (*(*requests)[groupTimestamp])[groupID]->insert(studentID);
    }
//    printf("after add %i\n", requests->size());
}

void ArbitersQueue::removeRequest(int groupTimestamp, int groupID, int studentID) {
//    printf("before remove %i\n", requests->size());
    if (requests->find(groupTimestamp) != requests->end()) {
//        printf("erase1\n");
        if ((*requests)[groupTimestamp]->find(groupID) != (*requests)[groupID]->end()) {
//            printf("erase2\n");
            if ((*(*requests)[groupTimestamp])[groupID]->find(studentID) !=
                (*(*requests)[groupTimestamp])[groupID]->end()) {
//                printf("erase3\n");
                (*(*requests)[groupTimestamp])[groupID]->erase(studentID);
                if ((*(*requests)[groupTimestamp])[groupID]->size() == 0) {
//                    printf("erase4\n");
                    delete (*(*requests)[groupTimestamp])[groupID];
                    (*requests)[groupTimestamp]->erase(groupID);
                    if ((*requests)[groupTimestamp]->size() == 0) {
                        delete (*requests)[groupTimestamp];
                        requests->erase(groupTimestamp);
//                        printf("erase\n");
                    }
                }
            }
        }
    }
//    printf("after remove %i\n", requests->size());
}

bool ArbitersQueue::canGetArbiter(int groupTimestamp, int groupID, int studentID) {
    if (requests->find(groupTimestamp) != requests->end()) {
        if ((*requests)[groupTimestamp]->find(groupID) != (*requests)[groupID]->end()) {
            if ((*(*requests)[groupTimestamp])[groupID]->find(studentID) ==
                (*(*requests)[groupTimestamp])[groupID]->end()) {
                int count = 0;
                for (auto &e: *requests) {
                    if (e.first < groupTimestamp) {
                        count++;
                    }
                    if (e.first == groupTimestamp) {
                        for (auto &e2: *(*requests)[groupTimestamp]) {
                            if (e2.first < groupID) {
                                count++;
                            }
                        }
                    }
                }
//                printf("count: %i\n", count);
//                printf("after count %i\n", requests->size());
                if (count < arbitersNumber) {
                    return true;
                }
            }
        }
    }
    return false;
}

ArbitersQueue::~ArbitersQueue() {
    for (auto &e: *requests) {
        for (auto &e2: *(*requests)[e.first]) {
            delete e2.second;
        }
        delete e.second;
    }
    delete requests;
}









