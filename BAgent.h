#ifndef BAGENT_H_
#define BAGENT_H_

#include "agent.h"

#include "timer-handler.h"

class BAgent : public Agent {
public:
	BAgent();
	int command(int argc, const char*const* argv);
	int myVal_;
	int myState_[100];
	int id_;

	void recv(Packet* p, Handler*);

	void myTimeout();

	class MyTimer : public TimerHandler {
	public:
		MyTimer(BAgent &a) : agent(a) {}
		void expire(Event *e) { //agent.myTimeout();
		}
		BAgent &agent;
	} myTimer;

};

#endif /*CLUSTERINGAGENT_H_*/
