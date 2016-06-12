/*
 * State.h
 *
 *  Created on: 08-06-2016
 *      Author: debian
 */

#ifndef STATE_H_
#define STATE_H_

#include "Student.h"

class State {
private:
	Student *student;


public:
	State(Student *student);
	virtual ~State();
};

#endif /* STATE_H_ */
