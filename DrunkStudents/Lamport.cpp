/*
 * Lamport.cpp
 *
 *  Created on: 08-06-2016
 *      Author: debian
 */

#include "Lamport.h"
#include <algorithm>

Lamport::Lamport() {
    timestamp = 0;
}

Lamport::~Lamport() {
}

void Lamport::increment() {
    timestamp++;
}

void Lamport::update(long receivedTimestamp) {
    this->timestamp = std::max(receivedTimestamp, timestamp) + 1;
}

long Lamport::getTimestamp() {
    return timestamp;
}

