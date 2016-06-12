/*
 * Lamport.h
 *
 *  Created on: 08-06-2016
 *      Author: debian
 */

#ifndef LAMPORT_H_
#define LAMPORT_H_

class Lamport {

private:
	long timestamp;

public:
	Lamport();
	virtual ~Lamport();

	void increment();
	void update(long receivedTimestamp);
	long getTimestamp();

};

#endif /* LAMPORT_H_ */
